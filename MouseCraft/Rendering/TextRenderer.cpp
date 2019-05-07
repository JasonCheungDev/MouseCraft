#include "TextRenderer.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include "../GL/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}



TextRenderer::TextRenderer()
{
	// initialize default shader
	textShader = new Shader("shaders/text_vertex.glsl", "shaders/text_fragment.glsl");

	// initialize FreeType (for loading font files) 
	if (FT_Init_FreeType(&ft))
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// create default font 
	LoadFont(FONT_MANAGER_DEFAULT_FONT);

	// create opengl buffers
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::SetScreenSize(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	uiProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}

void TextRenderer::LoadFont(std::string path)
{
	// don't reload a font face
	if (_fonts.find(path) != _fonts.end())
	{
		std::cout << "INFO::FREETYPE: Font is already loaded - " << path << std::endl;
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, path.c_str(), 0, &face))
	{
		std::cerr << "ERROR::FREETYPE: Failed to load font " << path << std::endl;
		return;
	}
	FT_Set_Pixel_Sizes(face, FREETYPE_DYNAMIC_WIDTH, 48);

	// first pass to determine overall texture size 
	unsigned int w = 0;
	unsigned int h = 0;

	for (GLubyte c = 32; c < 128; c++) 
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) 
		{
			std::cerr << "ERROR: Loading character " << c << " failed!" << std::endl;
			continue;
		}
		w += face->glyph->bitmap.width;
		h = std::max(h, face->glyph->bitmap.rows);
	}

	float atlas_width = w;
	float atlas_height = h;

	// create texture 
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// fill in atlas 
	int x = 0;

	for (GLubyte c = 32; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;

		glTexSubImage2D(
			GL_TEXTURE_2D,					// texture  
			0,								// level
			x,								// offset 
			0,								// offset 
			face->glyph->bitmap.width,		// width 
			face->glyph->bitmap.rows,		// height 
			GL_RED,							// format
			GL_UNSIGNED_BYTE,				// type 
			face->glyph->bitmap.buffer		// data
		);

		// calculate texture coordinates 
		float left, top, bottom, right;
		left   = x / atlas_width;
		right  = (x + face->glyph->bitmap.width) / atlas_width;
		top    = 0;
		bottom = (face->glyph->bitmap.rows) / atlas_height;

		// store character for later use
		AtlasCharacter character = {
			glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			glm::vec4(left, top, right, bottom),
			face->glyph->advance.x >> 6
		};
		_fonts[path].Characters.insert(std::pair<GLchar, AtlasCharacter>(c, character));
	
		x += face->glyph->bitmap.width;
	}
	_fonts[path].LineHeight = face->size->metrics.height >> 6;
	_fonts[path].Path = path;
	_fonts[path].Size = glm::vec2(atlas_width, atlas_height);
	_fonts[path].TexId = tex;

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
}

void TextRenderer::RenderText(std::string text, GLfloat x, GLfloat y, glm::vec3 color, GLfloat scale, TextAlignment alignment, Shader * s, std::string font)
{
	if (width == 0 || height == 0)
	{
		std::cerr << "ERROR: TextRenderer screen width/height is not set yet!" << std::endl;
		return;
	}

	if (text.empty()) 
		return;

	auto tm = GenerateTextMesh(text, font, alignment);

	if (tm == nullptr)
	{
		std::cerr << "ERROR: TextRenderer failed to generate TextMesh!" << std::endl;
		return;
	}

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);


	// Activate corresponding render state	
	if (s == nullptr) s = textShader;
	s->use();
	s->setVec3("u_TextColor", color);
	s->setMat4("u_Projection", uiProjection);
	glBindVertexArray(textVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tm->TexId);

	auto a = sizeof(tm->Mesh);
	auto b = sizeof(glm::vec4) * tm->Mesh.size();
	auto c = tm->Mesh[0];

	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * tm->Mesh.size(), &tm->Mesh[0], GL_DYNAMIC_DRAW);

	auto d = glGetError();

	glDrawArrays(GL_TRIANGLES, 0, tm->Mesh.size());

	glBindTexture(GL_TEXTURE_2D, 0);

}

void TextRenderer::RenderText(TextMesh * textMesh, GLfloat x, GLfloat y, glm::vec3 color, GLfloat scale, Shader * s)
{
}

TextMesh* TextRenderer::GenerateTextMesh(std::string text, std::string font, TextAlignment alignment)
{
	if (text.empty()) 
		return nullptr;

	// Retrieve font information 
	if (_fonts.find(font) == _fonts.end())
	{
		std::cerr << "ERROR: Failed to generate TextMesh. Font not loaded: " << font << std::endl;
		return nullptr;
	}
	
	return GenerateTextMesh(text, _fonts[font], alignment);
}

TextMesh * TextRenderer::GenerateTextMesh(std::string text, AtlasFont& fontInfo, TextAlignment alignment)
{
	// Initialize TextMesh
	TextMesh* tm = new TextMesh();

	float width, height = 0;// final size of text mesh
	float x, y = 0;				// current position of character
	int charCount = 0;		// number of characters

	// determine number of characters for mesh allocation
	for (auto& c : text)
		if (c != '\n')
			++charCount;
	tm->Mesh.reserve(charCount * 6);

	// Split text into lines 
	std::stringstream ss(text);
	std::string line;
	charCount = 0;
	while (std::getline(ss, line, '\n'))
	{
		// iterate thru all characters to determine horizontal size
		float lineWidth = 0;
		std::string::const_iterator c;
		for (c = line.begin(); c != line.end(); c++)
			lineWidth += fontInfo.Characters[*c].Advance;
		width = std::max(width, lineWidth);
		height += fontInfo.LineHeight;

		// determine start position
		if (alignment == TextAlignment::Left)
			x = 0;
		else if (alignment == TextAlignment::Center)
			x = -(lineWidth / 2);
		else if (alignment == TextAlignment::Right)
			x = -lineWidth;

		// Iterate through all characters
		for (c = line.begin(); c != line.end(); c++)
		{
			AtlasCharacter& ch = fontInfo.Characters[*c];

			GLfloat xpos = x + ch.Bearing.x;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y);
			GLfloat w = ch.Size.x;
			GLfloat h = ch.Size.y;

			// Update VBO for each character
			tm->Mesh.emplace_back(xpos		, ypos + h	, ch.TexCoords[0], ch.TexCoords[1]);
			tm->Mesh.emplace_back(xpos		, ypos		, ch.TexCoords[0], ch.TexCoords[3]);
			tm->Mesh.emplace_back(xpos + w	, ypos		, ch.TexCoords[2], ch.TexCoords[3]);
			tm->Mesh.emplace_back(xpos		, ypos + h	, ch.TexCoords[0], ch.TexCoords[1]);
			tm->Mesh.emplace_back(xpos + w	, ypos		, ch.TexCoords[2], ch.TexCoords[3]);
			tm->Mesh.emplace_back(xpos + w	, ypos + h	, ch.TexCoords[2], ch.TexCoords[1]);

			++charCount;
			x += ch.Advance;
		}

		// shift drawing position down for next line
		x = 0;
		y -= fontInfo.LineHeight;
	}

	// center mesh 
	float xOffset = 0;
	float yOffset = height / 2;
	if (alignment == TextAlignment::Left)
		xOffset = 0;
	else if (alignment == TextAlignment::Center)
		xOffset = -width / 2;
	else if (alignment == TextAlignment::Right)
		xOffset = width / 2;

	for (auto& vertex : tm->Mesh)
	{
		vertex.x = vertex.x + xOffset;
		vertex.y = vertex.y + yOffset;
	}

	// finalize mesh
	tm->Alignment = alignment;
	tm->Size = { width, height };
	tm->TexId = fontInfo.TexId;
	return tm;
}

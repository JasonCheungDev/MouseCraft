#include "TextRenderer.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../GL/glad.h"
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
	textShader = new Shader("res/shaders/text_vertex.glsl", "res/shaders/text_fragment.glsl");

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
			{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
			{ face->glyph->bitmap_left, face->glyph->bitmap_top },
			{ left, top, right, bottom },
			face->glyph->advance.x >> 6
		};
		_fonts[path].Characters.insert(std::pair<GLchar, AtlasCharacter>(c, character));
	
		x += face->glyph->bitmap.width;
	}
	_fonts[path].Path = path;
	_fonts[path].TexId = tex;
	_fonts[path].Size = { atlas_width, atlas_height };
	_fonts[path].LineHeight = face->size->metrics.height >> 6;

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
}

void TextRenderer::UnloadFont(std::string path)
{
	_fonts.erase(path);
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

	// calculate information for actual rendering function
	auto tm = GenerateTextMesh(text, font, alignment);

	// correct alignment (Left == TopLeft, Center == TopCenter, Right == TopRight)
	y -= tm->Size.y / 2 * scale;
	if (alignment == TextAlignment::Left)
		x += tm->Size.x / 2 * scale;
	else if (alignment == TextAlignment::Right)
		x -= tm->Size.x / 2 * scale;

	glm::mat4 transformation = glm::mat4(1.0f);
	transformation = glm::translate(transformation, glm::vec3(x,y,0));
	transformation = glm::scale(transformation, glm::vec3(scale));

	RenderText(tm, transformation, color, s);
}

void TextRenderer::RenderText(TextMesh * textMesh, float x, float y, float scale, glm::vec3 color, Shader * s)
{
	RenderText(
		textMesh, 
		glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0)), glm::vec3(1.0f)), 
		color, 
		s);
}

void TextRenderer::RenderText(TextMesh* textMesh, glm::mat4 transformation, glm::vec3 color, Shader * s)
{
	if (width == 0 || height == 0)
	{
		std::cerr << "ERROR: TextRenderer screen width/height is not set yet!" << std::endl;
		return;
	}

	if (textMesh == nullptr)
	{
		std::cerr << "ERROR: TextRenderer can't render null TextMesh!" << std::endl;
		return;
	}

	// Activate corresponding render state	
	if (s == nullptr) s = textShader;
	s->use();
	s->setVec3("u_TextColor", color);
	s->setMat4("u_Projection", uiProjection);
	s->setMat4("u_Model", transformation);

	glBindVertexArray(textVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textMesh->TexId);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * textMesh->Mesh.size(), &textMesh->Mesh[0], GL_DYNAMIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, textMesh->Mesh.size());

	glBindTexture(GL_TEXTURE_2D, 0);
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

	// final size of text mesh
	float width = 0;
	float height = 0; 
	// current position of character
	float x = 0;
	float y = -(float)fontInfo.LineHeight;	
	// number of (non-newline) characters and counter
	int charCount = 0;				 

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
			tm->Mesh.emplace_back(xpos		, ypos + h	, ch.TexCoords.left  , ch.TexCoords.top   );
			tm->Mesh.emplace_back(xpos		, ypos		, ch.TexCoords.left  , ch.TexCoords.bottom);
			tm->Mesh.emplace_back(xpos + w	, ypos		, ch.TexCoords.right , ch.TexCoords.bottom);
			tm->Mesh.emplace_back(xpos		, ypos + h	, ch.TexCoords.left  , ch.TexCoords.top   );
			tm->Mesh.emplace_back(xpos + w	, ypos		, ch.TexCoords.right , ch.TexCoords.bottom);
			tm->Mesh.emplace_back(xpos + w	, ypos + h	, ch.TexCoords.right , ch.TexCoords.top   );

			++charCount;
			x += ch.Advance;
		}

		// shift drawing position down for next line
		x = 0;
		y -= fontInfo.LineHeight;
	}

	// correct origin point (center)
	float xOffset = 0;
	float yOffset = height / 2;
	if (alignment == TextAlignment::Left)
		xOffset = -width / 2;
	if (alignment == TextAlignment::Center)
		xOffset = 0;
	else if (alignment == TextAlignment::Right)
		xOffset = width / 2;
	
	for (auto& vertex : tm->Mesh)
	{
		vertex.x = vertex.x + xOffset;
		vertex.y = vertex.y + yOffset;
	}

	// finalize mesh
	tm->Text = text;
	tm->Font = fontInfo.Path;
	tm->Alignment = alignment;
	tm->Size = { width, height };
	tm->TexId = fontInfo.TexId;
	return tm;
}

#include "TextRenderer.h"

#include <iostream>

#include "Shader.h"
#include <sstream>
#include "../GL/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	auto it = fonts.find(path);
	if (it != fonts.end())
	{
		// element found;
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

	fonts[path].LineHeight = face->size->metrics.height;	// scaled height

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph for " << (char)c << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};

		fonts[path].Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
}

// adapted from https://learnopengl.com/In-Practice/Text-Rendering
void TextRenderer::RenderText(
	std::string text,
	TextAlignment alignment,
	GLfloat x, GLfloat y,
	GLfloat scale,
	glm::vec3 color,
	Shader* s,
	std::string font)
{
	if (width == 0 || height == 0)
	{
		std::cerr << "ERROR: FontManager screen width/height is not set yet!" << std::endl;
		return;
	}

	if (text.empty()) return;

	// Retrieve font information 
	if (font.empty())
	{
		// use default font 
		font = FONT_MANAGER_DEFAULT_FONT;
	}
	else
	{
		// check if font exist
		auto it = fonts.find(font);
		if (it == fonts.end())
		{
			std::cerr << "ERROR: Failed to draw text. Font not loaded: " << font << std::endl;
			return;
		}
	}
	const auto fontInfo = &fonts[font];

	// Activate corresponding render state	
	if (s == nullptr) s = textShader;
	s->use();
	s->setVec3("u_TextColor", color);
	s->setMat4("u_Projection", uiProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	// Split text into lines 
	std::stringstream ss(text);
	std::string line;
	GLfloat initialX = x;
	GLfloat horSize = 0;
	while (std::getline(ss, line, '\n'))
	{
		// iterate thru all characters to determine horizontal size
		std::string::const_iterator c;
		for (c = line.begin(); c != line.end(); c++)
		{
			Character ch = fontInfo->Characters[*c];
			// Bitshift by 6 to get value in pixels (2^6 = 64)
			horSize += (ch.Advance >> 6) * scale;
		}

		// determine start position
		if (alignment == TextAlignment::Left)
			x = x;	// mmmm great code.
		else if (alignment == TextAlignment::Center)
			x = x - (horSize / 2);
		else if (alignment == TextAlignment::Right)
			x = x - horSize;

		// Iterate through all characters
		for (c = line.begin(); c != line.end(); c++)
		{
			Character ch = fontInfo->Characters[*c];

			GLfloat xpos = x + ch.Bearing.x * scale;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;

			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, textVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}

		// shift drawing position down for next line
		y -= (fontInfo->LineHeight >> 6) * scale;
		// and reset x position 
		x = initialX;
		horSize = 0;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
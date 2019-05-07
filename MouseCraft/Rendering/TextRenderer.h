#pragma once

#include "UI/View.h"
#include "../GL/glad.h"
#include <glm/glm.hpp>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <string>
#include <map>
#include "Shader.h"

#define FONT_MANAGER_DEFAULT_FONT "res/fonts/arial.ttf"
#define FONT_MANAGER_DEFAULT_FONTSIZE 48
#define FREETYPE_DYNAMIC_WIDTH 0

// A freetype character 
struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

// Font information used to render 
struct FontInfo
{
	std::string Path;
	GLuint LineHeight;
	std::map<GLchar, Character> Characters;
};

struct FontAtlas
{
	std::string Path;
	GLuint LineHeight;
	
};

struct CharacterAtlas
{

};

class TextRenderer
{
// singleton pattern 
public:
	static TextRenderer& Instance()
	{
		static TextRenderer _instance;
		return _instance;
	}
	TextRenderer(TextRenderer const&) = delete;
	void operator=(TextRenderer const&) = delete;
private:
	TextRenderer();
	~TextRenderer();
	
// functions
public:
	void SetScreenSize(unsigned int width, unsigned int height);
	void LoadFont(std::string path);
	void UnloadFont(std::string path);
	void RenderText(std::string text, TextAlignment alignment, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, Shader *s = nullptr, std::string font = FONT_MANAGER_DEFAULT_FONT);

private:
	int width, height;
	FT_Library ft;
	std::map<std::string, FontInfo> fonts;
	unsigned int textVAO;	// quad to draw a single letter 
	unsigned int textVBO;	// quad buffer to draw a single letter
	Shader* textShader;			// default text shader 
	glm::mat4 uiProjection;
};


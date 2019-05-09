#pragma once

#include <string>

// General 
extern const std::string SHADER_MODEL;
extern const std::string SHADER_VIEW;
extern const std::string SHADER_PROJECTION;
extern const std::string SHADER_MODELVIEW;
extern const std::string SHADER_VIEW_POS;
// Framebuffers
extern const std::string SHADER_FB_POS;
extern const std::string SHADER_FB_NRM;
extern const std::string SHADER_FB_COL;
extern const std::string SHADER_FB_DPH;
extern const std::string SHADER_FB_SHD;
// Colors
extern const std::string SHADER_DIFFUSE;
extern const std::string SHADER_SPECULAR;			// not used
extern const std::string SHADER_AMBIENT;			// not used
extern const std::string SHADER_EMISSIVE;			// not used
// Material
extern const std::string SHADER_SHININESS;			// aka Specular intensity
extern const std::string SHADER_ROUGHNESS;			// not used 
extern const std::string SHADER_OPACITY;
// Textures 
extern const std::string SHADER_TEX_NONE;
extern const std::string SHADER_TEX_DIFFUSE;
extern const std::string SHADER_TEX_SPECULAR;
extern const std::string SHADER_TEX_NORMAL;
extern const std::string SHADER_TEX_HEIGHT;
extern const std::string SHADER_TEX_CUBEMAP;
// Lighting 
extern const std::string SHADER_LIGHTSPACE;
extern const std::string SHADER_LIGHT_POS;
extern const std::string SHADER_LIGHT_COLOR;
extern const std::string SHADER_LIGHT_RANGE;
extern const std::string SHADER_LIGHT_DIR;
extern const std::string SHADER_LIGHT_INTENSITY;	// not used (could mean fall off though?)
extern const std::string SHADER_LIGHT_AMBIENT_INTENSITY;
// Shadows
extern const std::string SHADER_SHADOW_NEAR;
extern const std::string SHADER_SHADOW_FAR;

class Constants
{
public:
	Constants() {};
	~Constants() {};
};


#include "Constants.h"
#include "../GL/glad.h"

#include <string>

// Values from AI_MATKEY should be mapped into string variable names that .glsl files can use. 
// http://assimp.sourceforge.net/lib_html/materials.html
// General 
const std::string SHADER_MODEL         = "u_Model";
const std::string SHADER_VIEW		   = "u_View";
const std::string SHADER_PROJECTION    = "u_Projection";
const std::string SHADER_VIEW_POS      = "u_ViewPosition";
// FrameBuffers
const std::string SHADER_FB_POS        = "u_PosTex";
const std::string SHADER_FB_NRM        = "u_NrmTex";
const std::string SHADER_FB_COL        = "u_ColTex";
const std::string SHADER_FB_DPH        = "u_DphTex";
const std::string SHADER_FB_SHD		   = "u_ShadowMap";
// Colors
const std::string SHADER_DIFFUSE	   = "u_Diffuse";				// color
const std::string SHADER_SPECULAR      = "u_Specular";				// color
const std::string SHADER_AMBIENT	   = "u_Ambient";				// color
const std::string SHADER_EMISSIVE	   = "u_Emisive";				// color
// Material
const std::string SHADER_SHININESS	   = "u_SpecularIntensity";     // >0
const std::string SHADER_ROUGHNESS     = "u_Roughness";             
const std::string SHADER_OPACITY	   = "u_Opacity";				// 0-1
// Textures
const std::string SHADER_TEX_NONE      = "u_NoTexture";				// bool 
const std::string SHADER_TEX_DIFFUSE   = "u_TexDiffuse";			// texture 
const std::string SHADER_TEX_SPECULAR  = "u_TexSpecular";			// texture 
const std::string SHADER_TEX_NORMAL    = "u_TexNormal";				// texture 
const std::string SHADER_TEX_HEIGHT    = "u_TexHeight";				// texture
const std::string SHADER_TEX_CUBEMAP   = "u_TexCubeMap";			// texture 
// Lighting 
const std::string SHADER_LIGHTSPACE    = "u_LightSpaceMatrix";
const std::string SHADER_LIGHT_POS	   = "u_LightPos";
const std::string SHADER_LIGHT_COLOR   = "u_LightColor";
const std::string SHADER_LIGHT_RANGE   = "u_LightRange";
const std::string SHADER_LIGHT_DIR     = "u_LightDir";
const std::string SHADER_LIGHT_INTENSITY = "u_LightIntensity";
const std::string SHADER_LIGHT_AMBIENT_INTENSITY = "u_AmbientIntensity";
// Shadows
const std::string SHADER_SHADOW_NEAR   = "u_ShadowNear";
const std::string SHADER_SHADOW_FAR    = "u_ShadowFar";

#include "Material.h"
#include "../GL/glad.h"
#include <stb_image.h>
#include <iostream>
#include "Constants.h"

Material::Material()
{
}

Material::~Material()
{
}

void Material::LoadMaterial(const Shader* shader)
{
	// load all uniform data 
	for (const auto& elem : uniformsVec3)
	{
		shader->setVec3(elem.first, elem.second);
	}

	for (const auto& elem : uniformsFloat)
	{
		shader->setFloat(elem.first, elem.second);
	}

	for (const auto& elem : uniformsInt)
	{
		shader->setInt(elem.first, elem.second);
	}
}

int Material::LoadMaterial(const Shader* shader, unsigned int pos)
{
	// load uniform data
	LoadMaterial(shader);
	// load texture data 
	if (textures.size() == 0)
	{
		shader->setBool(SHADER_TEX_NONE, true);
	}
	else
	{
		shader->setBool(SHADER_TEX_NONE, false);
		for (int i = 0; i < textures.size(); i++)
		{
			// Set the uniform to point to texture 
			shader->setInt(textures[i].uniform, pos + i);
			// Activate the uniform variable 
			glActiveTexture(GL_TEXTURE0 + pos + i);
			// Bind the texture 
			glBindTexture(GL_TEXTURE_2D, textures[i].texture->GetId());
			// std::cout << "Binding uniform to location: " << pos + i << " with name " << textures[i].uniform << std::endl;
		}
	}
	return textures.size();
}

void Material::AddTexture(TextureShaderInfo& info)
{
	textures.push_back(info);
}

void Material::AddTextures(const std::vector<TextureShaderInfo>& infos)
{
	textures.insert(textures.end(), infos.begin(), infos.end());
}

void Material::SetVec3(const char * uniform, glm::vec3 value)
{
	uniformsVec3[uniform] = value;
}

void Material::SetFloat(const char * uniform, float value)
{
	uniformsFloat[uniform] = value;
}

void Material::SetInt(const char * uniform, int value)
{
	uniformsInt[uniform] = value;
}

void Material::SetBool(const char * uniform, bool value)
{
	uniformsInt[uniform] = value;
}

/* Notes:
	// load source image to opengl
	glTexImage2D(
		GL_TEXTURE_2D,		// for 2D texture (not 1D or 3D)
		0,					// manual mipmap level. 0 for default.
		GL_RGB,				// data format. our albedo does not support alpha.
		width,				// width of texture
		height,				// height of texture
		0,					// legacy. always set to 0.
		GL_RGB,				// data format of source. jpg has no alpha.
		GL_UNSIGNED_BYTE,	// data format of source. stbi_load reads as bytes.
		data				// data source.
	);
*/
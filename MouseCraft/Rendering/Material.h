#pragma once

#include <vector>
#include <utility>
#include <string>
#include <map>
#include <assimp\material.h>
#include "Shader.h"
#include "Texture.h"


/// <summary>
/// Customizable settings for a shader program. Each shader 
/// should have one material type. 
/// </summary>
class Material
{
// variables 
protected: 
	/// Shader that this material is for 
	// Shader* shader;						

	/// Textures that this material has 
	std::vector<TextureShaderInfo> textures;	// todo: use shared pointers for automatic gc 
	// std::pair<std::string, unsigned int> textures; 

	/// Stores uniform values in the form of vector 3.
	/// The key is the variable name in shader.
	std::map<std::string, glm::vec3> uniformsVec3;

	/// Stores uniform values in the form floats.
	/// The key is the variable name in shader.
	std::map<std::string, float> uniformsFloat;

	/// Stores uniform values in the form of integers (and booleans).
	/// The key is the variable name in shader.
	std::map<std::string, int> uniformsInt;

// functions 
public:
	Material();
	~Material();

	/// <summary>
	/// Loads material settings into opengl shader program.
	/// Highly recommended to call LoadMaterial(shader,textureLoc) even if you have no texture.
	/// </summary>
	virtual void LoadMaterial(const Shader* shader);

	/// <summary>
	/// Loads material settings into opengl shader program
	/// including any textures. 
	/// </summary>
	/// <param name="texStartLoc">Start address of the first free texture location.</param>
	/// <return>Number of textures bound.</return>
	virtual int LoadMaterial(const Shader* shader, unsigned int texStartLoc);

	void AddTexture(TextureShaderInfo& info);

	void AddTexture(const std::string& uniform, Texture* texture);

	void AddTextures(const std::vector<TextureShaderInfo>& infos);

	void SetVec3(const char* uniform, glm::vec3 value);

	void SetFloat(const char* uniform, float value);

	void SetInt(const char* uniform, int value);

	void SetBool(const char* uniform, bool value);
};


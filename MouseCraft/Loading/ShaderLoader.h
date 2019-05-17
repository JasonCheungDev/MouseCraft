#pragma once

#include <string>
#include "ResourceCache.h"
#include "JsonLoader.h"
#include "MaterialLoader.h"
#include "../Rendering/Shader.h"
#include <json.hpp>
using json = nlohmann::json;

class ShaderLoader
{
public:
	// Loads a shader from json. 
	// Will return a preloaded shader if found.
	// To directly load a shader program use new Shader(v_path, f_path).
	static Shader* Load(const std::string& jsonPath)
	{
		Shader* shader = ResourceCache<Shader>::Instance().Get(jsonPath);
		if (shader)
			return shader;
		else
			return LoadFromJson(*JsonLoader::Load(jsonPath));
	}

	// Loads a shader from json.
	// Recommended to use Load() instead. 
	static Shader* LoadFromJson(json json)
	{
		Shader* shader = new Shader(
			json["vertex"].get<std::string>().c_str(), 
			json["fragment"].get<std::string>().c_str());

		// set default material if found
		if (json.find("material") != json.end())
		{
			auto defaulTMaterial = MaterialLoader::Load(json["material"].get<std::string>());
			shader->defaultSettings = defaulTMaterial.get();
		}

		return shader;
	}
};


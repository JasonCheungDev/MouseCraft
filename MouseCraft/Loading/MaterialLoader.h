#pragma once

#include <fstream>
#include "../Rendering/Material.h"
#include "TextureLoader.h"
#include "ResourceCache.h"
#include <json.hpp>
using json = nlohmann::json;

class MaterialLoader
{
public:
	MaterialLoader();
	~MaterialLoader();

	// Loads a material from path.
	// Will return a preloaded texture if found.
	static std::shared_ptr<Material> Load(const std::string & path)
	{
		auto material = ResourceCacheShared<Material>::Instance().Get(path);

		if (material)
		{
			return material;
		}
		else
		{
			std::ifstream ifs(path);
			if (!ifs.good())
			{
				std::cerr << "ERROR: PrefabLoader could not find component file: " << path << std::endl;
				return nullptr;
			}
			json json;
			ifs >> json;

			return LoadFromJson(json);
		}
	}

	// Loads a material from path.
	// Creates a new material. (Use Load() if you want a preloaded one).
	static std::shared_ptr<Material> LoadFromJson(json json)
	{
		std::shared_ptr<Material> material = std::make_shared<Material>();

		auto vec3s = json["vec3s"];
		for (auto& vec3 : vec3s)
		{
			material->SetVec3(vec3["name"].get<std::string>().c_str(),
				glm::vec3(vec3["value"][0].get<float>(), vec3["value"][1].get<float>(), vec3["value"][2].get<float>()));
		}

		auto floats = json["floats"];
		for (auto& f : floats)
		{
			material->SetFloat(f["name"].get<std::string>().c_str(), f["value"].get<float>());
		}

		auto ints = json["ints"];
		for (auto& i : ints)
		{
			material->SetInt(i["name"].get<std::string>().c_str(), i["value"].get<int>());
		}

		auto textures = json["textures"];
		for (auto& t : textures)
		{
			auto tex = TextureLoader::Load(t["value"].get<std::string>());
			material->AddTexture(t["name"].get<std::string>(), tex);
		}

		return material;
	}
};


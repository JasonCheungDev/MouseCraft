#include "PrefabLoader.h"



ComponentRegistrar::ComponentRegistrar(const std::string & jsonKey, Component *(*func)(json))
{
	PrefabLoader::AddComponentLoader(jsonKey, func);
}

EntityRegistrar::EntityRegistrar(const std::string & jsonKey, Entity *(*func)(json))
{
	PrefabLoader::AddEntityLoader(jsonKey, func);
}



void PrefabLoader::DumpLoaders()
{
	std::cout << "===== PREFABLOADER - REGISTERED COMPONENT LOADERS =====" << std::endl;
	for (auto& kvp : *getMap())
		std::cout << kvp.first << std::endl;
	std::cout << "===== PREFABLOADER - REGISTERED ENTITY LOADERS =====" << std::endl;
	for (auto& kvp : *getEntityMap())
		std::cout << kvp.first << std::endl;
	std::cout << "===== END =====" << std::endl;
}

void PrefabLoader::AddComponentLoader(const std::string & jsonKey, Component *(*func)(json))
{
	getMap()->insert(std::make_pair(jsonKey, func));
}

void PrefabLoader::AddEntityLoader(const std::string & jsonKey, Entity *(*func)(json))
{
	getEntityMap()->insert(std::make_pair(jsonKey, func));
}

Entity * PrefabLoader::LoadPrefab(std::string path)
{
	std::string* data = ResourceCache<std::string>::Instance().Get(path);
	json json;

	// check if prefab was loaded before (avoid accessing disk)
	if (data == nullptr)
	{
		// open file and parse json
		std::ifstream ifs(path);
		if (!ifs.good())
		{
			std::cerr << "ERROR: PrefabLoader could not find file: " << path << std::endl;
			return nullptr;
		}
		std::stringstream ss;
		ss << ifs.rdbuf();
		ResourceCache<std::string>::Instance().Add(path, new std::string(ss.str()));

		json = json::parse(ss.str());
	}
	else
	{
		json = json::parse(*data);
	}

	// load prefab 
	auto parent = Load(json);

	// return result
	return parent;
}

Component * PrefabLoader::LoadComponent(std::string path)
{
	std::string* data = ResourceCache<std::string>::Instance().Get(path);
	json json;

	// check if prefab was loaded before (avoid accessing disk)
	if (data == nullptr)
	{
		// open file and parse json
		std::ifstream ifs(path);
		if (!ifs.good())
		{
			std::cerr << "ERROR: PrefabLoader could not find component file: " << path << std::endl;
			return nullptr;
		}
		std::stringstream ss;
		ss << ifs.rdbuf();
		ResourceCache<std::string>::Instance().Add(path, new std::string(ss.str()));

		json = json::parse(ss.str());
	}
	else
	{
		json = json::parse(*data);
	}

	// use custom loader 
	auto loader = getMap()->find(json["type"]);
	if (loader != getMap()->end())
	{
		auto c = loader->second(json);
		if (json.find("enabled") != json.end())
		{
			c->SetEnabled(json["enabled"].get<bool>());
		}
		return c;
	}
	else
	{
		throw "ERROR: PrefabLoader component type not registered";
	}
}

Entity * PrefabLoader::Load(json json)
{
	// entity & transform 
	Entity* e = nullptr;

	// has a custom loader to use 
	if (json.find("loader") != json.end())
	{
		// use custom loader 
		auto loader = getEntityMap()->find(json["loader"]["type"].get<std::string>());
		if (loader != getEntityMap()->end())
		{
			e = loader->second(json["loader"]);
		}
		else
		{
			throw "ERROR: PrefabLoader entity loader not registered: " + json["loader"]["type"].get<std::string>();
		}
	}
	else
	{
		e = EntityManager::Instance().Create();
	}

	auto pos = glm::vec3(json["pos"][0].get<double>(), json["pos"][1].get<double>(), json["pos"][2].get<double>());
	auto rot = glm::radians(glm::vec3(json["rot"][0].get<double>(), json["rot"][1].get<double>(), json["rot"][2].get<double>()));
	auto scl = glm::vec3(json["scl"][0].get<double>(), json["scl"][1].get<double>(), json["scl"][2].get<double>());
	e->transform.setLocalPosition(pos);
	e->transform.setLocalRotation(rot);
	e->transform.setLocalScale(scl);

	// components 
	auto components = json["components"];
	for (auto& j : components)
	{
		// use custom loader 
		auto loader = getMap()->find(j["type"]);
		if (loader != getMap()->end())
		{
			e->AddComponent(loader->second(j));
		}
		else
		{
			throw "ERROR: PrefabLoader component type not registered";
		}
	}

	// children 
	auto children = json["entities"];
	for (auto& j : children)
	{
		e->AddChild(Load(j));
	}

	return e;
}

Entity * PrefabLoader::EntityRegistrarLoader(json json)
{
	return LoadPrefab(json["path"].get<std::string>());
}

PrefabLoader::ComponentMap* PrefabLoader::componentMap;

PrefabLoader::EntityMap* PrefabLoader::entityMap;

EntityRegistrar PrefabLoader::reg("prefab", &PrefabLoader::EntityRegistrarLoader);

#pragma once

#include "../Core/Component.h"
#include "../Core/Entity.h"
#include "../Core/EntityManager.h"
#include <json.hpp>
#include "../Loading/ResourceCache.h"
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
using json = nlohmann::json;


// Helper class to automatically register a Component class to the PrefabLoader 
struct ComponentRegistrar
{
	ComponentRegistrar(const std::string& jsonKey, Component*(*func)(json));;
};

// Helper class to automatically register a custom Entity loader to the PrefabLoader 
struct EntityRegistrar
{
	EntityRegistrar(const std::string& jsonKey, Entity*(*func)(json));;
};

// Loads an entity with components and child entities specified by a json file. 
class PrefabLoader
{
public:
	// Dumps all registered loaders
	static void DumpLoaders();

	// Manually add a component loader
	static void AddComponentLoader(const std::string& jsonKey, Component*(*func)(json));

	// Manually add an entity loader 
	static void AddEntityLoader(const std::string& jsonKey, Entity*(*func)(json));

	// Load an entity with a json file
	static Entity* LoadPrefab(std::string path);;

	// Load a component with a json file
	static Component* LoadComponent(std::string path);

private:
	// Recursive helper function to load an entity
	static Entity* Load(json json);

private:
	// datatype for string_key -> loader()
	typedef std::map<std::string, Component*(*)(json)> ComponentMap;
	typedef std::map<std::string, Entity*(*)(json)> EntityMap;

	static ComponentMap* getMap()
	{
		if (!componentMap)
			componentMap = new ComponentMap;
		return componentMap;
	}
	static EntityMap* getEntityMap()
	{
		if (!entityMap)
			entityMap = new EntityMap;
		return entityMap;
	}

	static ComponentMap* componentMap;
	static EntityMap* entityMap;

	//register self to allow prefabs to load other prefabs
	static Entity* EntityRegistrarLoader(json json);
	static EntityRegistrar reg;
};





/* Below doesn't work as we use CM's. With the updated ComponentFactory it would work.
// used for generic component ctor (doesn't work as we use CM's)
template<typename T> Component* CreateComponent(json json)
{
	return new T(json);
}

// Helper class to automatically register a Component class to the PrefabLoader 
template<typename T>
struct PrefabRegistrar : PrefabLoader
{
	// note: Doesn't actually inherit from PrefabLoader just need access to the ComponentMap
public:
	PrefabRegistrar(const std::string& jsonKey)
	{
		getMap()->insert(std::make_pair(jsonKey, &CreateComponent<T>));
	};
};
*/
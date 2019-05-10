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

// datatype for string_key -> loader()
typedef std::map<std::string, Component*(*)(json)> ComponentMap;
typedef std::map<std::string, Entity*(*)(json)> EntityMap;

// Loads an entity with components and child entities specified by a json file. 
class PrefabLoader
{
public:
	PrefabLoader();
	~PrefabLoader();
	
	// Dumps all registered loaders
	static void DumpLoaders();

	// Load an entity with a json file
	static Entity* LoadPrefab(std::string path);;

	// Load a component with a json file
	static Component* LoadComponent(std::string path);

private:
	// Recursive helper function to load an entity
	static Entity* Load(json json, Entity* parent);

protected:
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
private:
	static ComponentMap* componentMap;
	static EntityMap* entityMap;
};

// Helper class to automatically register a Component class to the PrefabLoader 
struct ComponentRegistrar : PrefabLoader
{
// note: Doesn't actually inherit from PrefabLoader just need access to the ComponentMap
public:
	ComponentRegistrar(const std::string& jsonKey, Component*(*func)(json))
	{
		getMap()->insert(std::make_pair(jsonKey, func));
	};
};

// Helper class to automatically register a custom Entity loader to the PrefabLoader 
struct EntityRegistrar : PrefabLoader
{
// note: Doesn't actually inherit from PrefabLoader just need access to the ComponentMap
public:
	EntityRegistrar(const std::string& jsonKey, Entity*(*func)(json))
	{
		getEntityMap()->insert(std::make_pair(jsonKey, func));
	};
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
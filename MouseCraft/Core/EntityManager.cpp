#include "EntityManager.h"

EntityManager::EntityManager()
{
	EventManager::Subscribe(EventName::ENTITY_CREATED, this);
	EventManager::Subscribe(EventName::ENTITY_DESTROYED, this);
}

EntityManager::~EntityManager()
{
	EventManager::Unsubscribe(EventName::ENTITY_CREATED, this);
	EventManager::Unsubscribe(EventName::ENTITY_DESTROYED, this);
}

Entity* EntityManager::Create()
{
	return new Entity();
}

Entity* EntityManager::Find(const std::string & name)
{
	for (const auto& e : entities)
		if (e->name == name)
			return e;
	return nullptr;
}

Entity* EntityManager::Find(const std::string & name, Entity* start)
{
	if (start->name == name)
		return start;
	
	Entity* found = nullptr;
	for (const auto& e : start->GetChildren())
	{
		found = Find(name, e);
		if (found) return found;
	}
	return found;
}

const std::vector<Entity*>& EntityManager::GetEntities()
{
	return entities;
}

void EntityManager::Notify(EventName eventName, Param * params)
{
	auto entityParam = static_cast<TypeParam<Entity*>*>(params);
	if (eventName == EventName::ENTITY_CREATED)
	{
		entities.push_back(entityParam->Param);
	}
	else if (eventName == EventName::ENTITY_DESTROYED)
	{
		entities.erase(
			std::remove(entities.begin(), entities.end(), entityParam->Param),
			entities.end());
	}
}

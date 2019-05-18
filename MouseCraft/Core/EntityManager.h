#pragma once

#include <vector>
#include "Entity.h"
#include "../Event/EventManager.h"

// Convenience class to retrieve all entities (including ones 
// not in the scene root) in a flat vector. 
class EntityManager : public ISubscriber
{
public:
// singleton 
public:
	static EntityManager& Instance()
	{
		static EntityManager _instance;
		return _instance;
	}
	EntityManager(EntityManager const&) = delete;
	void operator=(EntityManager const&) = delete;
private:
	EntityManager();
	~EntityManager();

// variables 
private:
	std::vector<Entity*> entities;

// functions 
public:
	Entity* Create();

	Entity* Find(const std::string& name);

	Entity* Find(const std::string& name, Entity* start);

	const std::vector<Entity*>& GetEntities();

	virtual void Notify(EventName eventName, Param *params);
};

/*
Notes: 
This class does not manage the lifecycle of the entity in any way 
(may change in the future). This is b/c the Entity already has strong 
intentional coupling with the Engine (requires knowledge of active scene 
for active status and engnie for deferred exeuction handling). 

Considering changing this in the future to aid cohesion but this 
would increase coupling from 2 (Entity+Engine) 
to 3 (Engine + EntityManager + SceneManager? + Entity? if abstracting Entity.Destroy() etc.) 
*/
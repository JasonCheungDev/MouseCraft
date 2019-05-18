#pragma once
#include <vector>
#include <utility>
#include <set>
#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include "PhysObjectType.h"
#include "CContactListener.h"
#include "AreaQueryCallback.h"
#include "RayQueryCallback.h"
#include "../Core/System.h"
#include "../Core/Entity.h"
#include "../Event/ISubscriber.h"
#include "../Util/CpuProfiler.h"

constexpr auto GRAVITY = -12;

class PhysicsComponent;

class CContactListener;

class PhysicsManager : public System
{
// singleton pattern 
public:
	static PhysicsManager& Instance()
	{
		static PhysicsManager _instance;
		return _instance;
	}
	PhysicsManager(PhysicsManager const&) = delete;
	void operator=(PhysicsManager const&) = delete;
private:
	PhysicsManager(); 
	~PhysicsManager();

// functions 
public:
	void FixedUpdate(float dt, int steps) override;
	
	std::vector<PhysicsComponent*> areaCheck(glm::vec2 bl, glm::vec2 tr, std::set<PhysObjectType::PhysObjectType>* toCheck = nullptr);
	
	PhysicsComponent* rayCheck(glm::vec2 start, glm::vec2 dest, glm::vec2& hit, std::set<PhysObjectType::PhysObjectType>* toCheck = nullptr);
	
	// Used to finalize creating a PhysicsComponent. Will bind the component to a physics body.
	// WARNING: Should only be called internally by the engine.
	void InitializeBody(PhysicsComponent* physicsComponent, float x, float y, float r = 0.0f);

private:
	void checkCollisions();

// variables
private:
	b2World *world;
	CContactListener *cListener;
	CpuProfiler profiler;
};
#pragma once
#include <set>
#include <Box2D/Box2D.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <json.hpp>
#include "PhysObjectType.h"
#include "../Core/Transform.h"
#include "../Core/Component.h"
#include "../Event/Subject.h"
#include "../Loading/PrefabLoader.h"

using json = nlohmann::json;

class PhysicsComponent : public Component
{
// functions 
public:
	PhysicsComponent(float w, float h, Transform& transform);
	PhysicsComponent(float w, float h, float x = 0.0f, float y = 0.0f, float r = 0.0f);
	~PhysicsComponent();
	
	// Sets the entity position to where the body is right now
	void initPosition();
	
	// Teleports the physics body to specified location (which will move the entity as well)
	void moveBody(glm::vec2 pos, float angle);

	// Note: Too lazy to make an enum and don't want to expose physics implementation (box2d)

	// Makes body dynamic 
	void makeDynamic();

	// Makes body static 
	void makeStatic();
	
	// Makes body kinematic 
	void makeKinematic();

	// Makes body no longer collide with anything but still simulate movement.
	// This action is irreversible (at the moment).
	void removeCollisions();

// variables
public: 
	glm::vec2 velocity;
	float rotation, width, height;
	b2Body* body;
	PhysObjectType::PhysObjectType pType;
	Subject<> stopMoving;
	Subject<> resumeMoving;
	Subject<PhysicsComponent*> onCollide;	//for collision between bodies
	Subject<PhysicsComponent*> onHit;		//for hitbox checking
	Subject<PhysicsComponent*> onBounce;	//for hitbox checking

private:
	static Component* Create(json json);
	static ComponentRegistrar reg;
};
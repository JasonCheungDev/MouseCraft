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

// Physically simulate this entity's transform.
// WARNING: Should always be placed on root (or under an entity with identity transform). 
class PhysicsComponent : public Component
{
// functions 
public:
	// Creates a physics component at entity's LOCAL position (on initialization).
	PhysicsComponent(float w, float h, bool startAtTransform);

	// Creates a physics component at transform location.
	PhysicsComponent(float w, float h, Transform& transform);

	// Create a physics component at physics world location.
	PhysicsComponent(float w, float h, float x = 0.0f, float y = 0.0f, float r = 0.0f);
	
	~PhysicsComponent();
	
	void OnInitialized() override;

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

	void ApplyForce(glm::vec2 force);

	void ApplyAngularForce(float force);

// variables
public: 
	glm::vec2 velocity;
	float rotation, width, height;
	b2Body* body;
	PhysObjectType::PhysObjectType pType;
	bool startAtWorld = false;
	Subject<> stopMoving;
	Subject<> resumeMoving;
	Subject<PhysicsComponent*> onCollide;	//for collision between bodies
	Subject<PhysicsComponent*> onHit;		//for hitbox checking
	Subject<PhysicsComponent*> onBounce;	//for hitbox checking

private:

	/*
	{
		"type": "Physics",
		"size": [1.0,1.0],			// width, height
		"body": "dynamic",			// dynamic, static, kinematic
		// OPTIONAL					// (if missing it will use entity position)
		"transform": [1.0,1.0,0.0],	// x, y, rotation
	}
	*/
	static Component* Create(json json);
	static ComponentRegistrar reg;
};
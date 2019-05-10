#include "PhysicsComponent.h"

#include "PhysicsManager.h"
#include "PhysicsUtil.h"

PhysicsComponent::PhysicsComponent(float w, float h, Transform & transform) :
	PhysicsComponent(w, h, transform.getWorldPosition().x, transform.getWorldPosition().y, transform.getWorldRotation().y)
{
}

PhysicsComponent::PhysicsComponent(float w, float h, float x, float y, float r) :
	width(w), height(h), rotation(r), velocity(), pType(PhysObjectType::NOTHING)
{
	// Connect with a body in the physics world
	PhysicsManager::Instance().InitializeBody(this, x, y);
}

PhysicsComponent::~PhysicsComponent()
{
	body->GetWorld()->DestroyBody(body);
}

void PhysicsComponent::initPosition()
{
	GetEntity()->transform.setLocalPosition(b2gl3(body->GetPosition()));
}

void PhysicsComponent::moveBody(glm::vec2 pos, float angle)
{
	body->SetTransform(gl2b2(pos), angle);
}

void PhysicsComponent::makeDynamic()
{
	body->SetType(b2BodyType::b2_dynamicBody);
	body->SetAwake(true);	// react to any collisions right away
}

void PhysicsComponent::makeStatic()
{
	body->SetType(b2BodyType::b2_dynamicBody);
}

void PhysicsComponent::makeKinematic()
{
	body->SetType(b2BodyType::b2_kinematicBody);
}

void PhysicsComponent::removeCollisions()
{
	b2Filter filter;
	filter.groupIndex = -1;
	body->GetFixtureList()->SetFilterData(filter);
}

Component* PhysicsComponent::Create(json json)
{
	// TODO:
	return nullptr;
	/*
	std::string physType = json["physType"].get<std::string>();
	PhysObjectType::PhysObjectType properType = PhysObjectType::NOTHING;

	if (physType == "OBSTACLE_UP")
		properType = PhysObjectType::OBSTACLE_UP;
	else if (physType == "OBSTACLE_DOWN")
		properType = PhysObjectType::OBSTACLE_DOWN;
	else if (physType == "CONTRAPTION_UP")
		properType = PhysObjectType::CONTRAPTION_UP;
	else if (physType == "CONTRAPTION_DOWN")
		properType = PhysObjectType::CONTRAPTION_DOWN;
	else if (physType == "PLATFORM")
		properType = PhysObjectType::PLATFORM;
	else if (physType == "PART")
		properType = PhysObjectType::PART;
	else if (physType == "PROJECTILE_UP")
		properType = PhysObjectType::PROJECTILE_UP;
	else if (physType == "PROJECTILE_DOWN")
		properType = PhysObjectType::PROJECTILE_DOWN;
	else if (physType == "CAT_UP")
		properType = PhysObjectType::CAT_UP;
	else if (physType == "CAT_DOWN")
		properType = PhysObjectType::CAT_DOWN;
	else if (physType == "MOUSE_UP")
		properType = PhysObjectType::MOUSE_UP;
	else if (physType == "MOUSE_DOWN")
		properType = PhysObjectType::MOUSE_DOWN;

	switch (properType)
	{
	case PhysObjectType::OBSTACLE_UP:
	case PhysObjectType::OBSTACLE_DOWN:
	case PhysObjectType::CONTRAPTION_UP:
	case PhysObjectType::CONTRAPTION_DOWN:
	case PhysObjectType::PLATFORM:
	case PhysObjectType::PART:
		return PhysicsManager::instance()->createGridObject(json["xPos"].get<float>(), json["yPos"].get<float>(),
			json["width"].get<int>(), json["height"].get<float>(), properType);
		break;
	case PhysObjectType::PROJECTILE_UP:
	case PhysObjectType::PROJECTILE_DOWN:
	case PhysObjectType::CAT_UP:
	case PhysObjectType::CAT_DOWN:
	case PhysObjectType::MOUSE_UP:
	case PhysObjectType::MOUSE_DOWN:
		return PhysicsManager::instance()->createObject(json["xPos"].get<float>(), json["yPos"].get<float>(),
			json["width"].get<float>(), json["height"].get<float>(), json["rotation"].get<float>(), properType);
		break;
	}
	
	return nullptr;
	*/
}

// !!! which key you want to load 
ComponentRegistrar PhysicsComponent::reg("PhysicsComponent", &PhysicsComponent::Create);
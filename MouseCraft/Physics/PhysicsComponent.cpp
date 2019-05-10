#include "PhysicsComponent.h"

#include "PhysicsManager.h"
#include "PhysicsUtil.h"
#include "../Loading/PrefabLoader.h"
#include "../Core/ComponentFactory.h"

PhysicsComponent::PhysicsComponent(float w, float h, bool startAtTransform)
	: PhysicsComponent(w, h, 0, 0, 0)
{
	startAtWorld = startAtTransform;
}

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

void PhysicsComponent::OnInitialized()
{
	if (startAtWorld)
		moveBody(glm::vec2(
			GetEntity()->t().wPos().x, 
			GetEntity()->t().wPos().z), 
			GetEntity()->t().wRot().y);
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
	body->SetType(b2BodyType::b2_staticBody);
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
	PhysicsComponent* component;

	float width = json["size"][0].get<float>();
	float height = json["size"][1].get<float>();
	
	if (json.find("transform") != json.end())
		component = ComponentFactory::Create<PhysicsComponent>(width, height, 
			json["transform"][0].get<float>(), json["transform"][1].get<float>(), json["transform"][2].get<float>());
	else
		component = ComponentFactory::Create<PhysicsComponent>(width, height, true);

	if (json["body"].get<std::string>() == "static")
		component->makeStatic();
	else if (json["body"].get<std::string>() == "kinematic")
		component->makeKinematic();

	return component;
}

// !!! which key you want to load 
ComponentRegistrar PhysicsComponent::reg("Physics", &PhysicsComponent::Create);
#include "PhysicsComponent.h"

#include "PhysicsManager.h"
#include "PhysicsUtil.h"
#include "../Loading/PrefabLoader.h"
#include "../Core/ComponentFactory.h"
#include "../Core/OmegaEngine.h"


PhysicsComponent::PhysicsComponent()
{
	// Note: that physics world is not notified yet. 
	startAtWorld = true;
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
	if (body) 
		body->GetWorld()->DestroyBody(body);
}

void PhysicsComponent::OnInitialized()
{
	if (startAtWorld)
	{
		auto scl = GetEntity()->transform.getWorldScale();
		auto rot = GetEntity()->transform.getWorldRotation();
		auto pos = GetEntity()->transform.getWorldPosition();

		width = scl.x;
		height = scl.z;
		rotation = -rot.y;
		
		// since we set world coordinates we need to place ourself on root.
		GetEntity()->SetParent(OmegaEngine::Instance().GetRoot());

		PhysicsManager::Instance().InitializeBody(this, pos.x, pos.z, rotation);
		
	}
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
	dynamics = Physics::Dynamics::DYNAMIC;
	body->SetType(b2BodyType::b2_dynamicBody);
	body->SetAwake(true);	// react to any collisions right away
}

void PhysicsComponent::makeStatic()
{
	dynamics = Physics::Dynamics::STATIC;
	body->SetType(b2BodyType::b2_staticBody);
}

void PhysicsComponent::makeKinematic()
{
	dynamics = Physics::Dynamics::KINEMATIC;
	body->SetType(b2BodyType::b2_kinematicBody);
}

void PhysicsComponent::removeCollisions()
{
	b2Filter filter;
	filter.groupIndex = -1;
	body->GetFixtureList()->SetFilterData(filter);
}

void PhysicsComponent::ApplyForce(glm::vec2 force)
{
	body->ApplyForce(gl2b2(force), body->GetWorldCenter(), true);
}

void PhysicsComponent::ApplyAngularForce(float force)
{
	body->ApplyTorque(force, true);
}

Component* PhysicsComponent::Create(json json)
{
	PhysicsComponent* component;

	if (json.find("size") != json.end())
	{
		float width = json["size"][0].get<float>();
		float height = json["size"][1].get<float>();

		component = ComponentFactory::Create<PhysicsComponent>(
			width, 
			height,
			json["transform"][0].get<float>(), // x
			json["transform"][1].get<float>(), // y
			json["transform"][2].get<float>()  // rot
		);
	}
	else
	{
		// use entity information 
		component = ComponentFactory::Create<PhysicsComponent>();
	}

	if (json["body"].get<std::string>() == "static")
		component->dynamics = Physics::Dynamics::STATIC;
	else if (json["body"].get<std::string>() == "kinematic")
		component->dynamics = Physics::Dynamics::KINEMATIC;

	return component;
}

// !!! which key you want to load 
ComponentRegistrar PhysicsComponent::reg("Physics", &PhysicsComponent::Create);
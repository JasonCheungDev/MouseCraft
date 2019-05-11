#include "PhysicsMover.h"


PhysicsMover::PhysicsMover()
{
	EventManager::Subscribe(EventName::INPUT_AXIS_2D, this);
}

PhysicsMover::~PhysicsMover()
{
	EventManager::Unsubscribe(EventName::INPUT_AXIS_2D, this);
}

void PhysicsMover::OnInitialized()
{
	physics = GetEntity()->GetComponent<PhysicsComponent>();
}

void PhysicsMover::FixedUpdate(float dt, int steps)
{
	physics->velocity = moveInput * speed;
	
}

void PhysicsMover::Notify(EventName eventName, Param * params)
{
	if (eventName == EventName::INPUT_AXIS_2D)
	{
		auto data = static_cast<TypeParam<Axis2DEvent>*>(params)->Param;

		if (data.axis == Axis::LEFT)
			moveInput = data.GetClamped();
		else if (data.axis == Axis::RIGHT)
			turnInput = data.GetClamped();
	}
}

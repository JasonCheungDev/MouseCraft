#include "FreeLookMovement.h"

#include "../Input/InputSystem.h"
#include "../Core/Entity.h"
#include <iostream>

FreeLookMovement::FreeLookMovement()
{
	EventManager::Subscribe(EventName::INPUT_AXIS_2D, this);
}


FreeLookMovement::~FreeLookMovement()
{
}

void FreeLookMovement::FixedUpdate(float deltaTime, int steps)
{
	glm::vec2 move = moveInput * moveSpeed * deltaTime;
	GetEntity()->t().translate(GetEntity()->t().forward() * -move.y);
	GetEntity()->t().translate(GetEntity()->t().right() * move.x);

	glm::vec2 turn = -turnInput * turnSpeed * deltaTime;
	GetEntity()->t().rotate(glm::vec3(turn.y, turn.x, 0));
}

void FreeLookMovement::Notify(EventName eventName, Param* params)
{
	if (eventName == EventName::INPUT_AXIS_2D)
	{
		auto data = static_cast<TypeParam<Axis2DEvent>*>(params)->Param;

		if (data.axis == Axis::LEFT)
			moveInput = data.GetClamped();
		else if (data.axis == Axis::RIGHT)
			turnInput = data.GetClamped();
	}

	UpdatableComponent::Notify(eventName, params);
}

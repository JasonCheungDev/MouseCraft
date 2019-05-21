#include "FreeLookMovement.h"

#include "../Input/InputSystem.h"
#include "../Core/Entity.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

FreeLookMovement::FreeLookMovement()
{
	EventManager::Subscribe(EventName::INPUT_AXIS_2D, this);
	EventManager::Subscribe(EventName::INPUT_BUTTON, this);
}


FreeLookMovement::~FreeLookMovement()
{
	EventManager::Unsubscribe(EventName::INPUT_AXIS_2D, this);
	EventManager::Unsubscribe(EventName::INPUT_BUTTON, this);
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

		if (data.axis == Axis::STICK_LEFT)
			moveInput = data.GetClamped();
		else if (data.axis == Axis::STICK_RIGHT)
			turnInput = data.GetClamped();
	}
	else if (eventName == EventName::INPUT_BUTTON)
	{
		auto data = static_cast<TypeParam<ButtonEvent>*>(params)->Param;

		if (data.button == Button::PRIMARY && data.isDown)
		{
			std::cout << "POS: " << glm::to_string(GetEntity()->transform.getWorldPosition()) << std::endl
				<< "ROT: " << glm::to_string(GetEntity()->transform.getWorldRotation()) << std::endl;
		}
	}
}

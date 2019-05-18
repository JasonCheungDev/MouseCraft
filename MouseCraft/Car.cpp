#include "Car.h"


Car::Car()
{
	EventManager::Subscribe(EventName::INPUT_AXIS_2D, this);
	EventManager::Subscribe(EventName::INPUT_BUTTON, this);
}

Car::~Car()
{
	EventManager::Unsubscribe(EventName::INPUT_AXIS_2D, this);
	EventManager::Unsubscribe(EventName::INPUT_BUTTON, this);
}

void Car::OnInitialized()
{
	physics = GetEntity()->GetComponent<PhysicsComponent>();
	// TODO:
	physics->body->SetAngularDamping(5.0f);
	physics->body->SetLinearDamping(2.0f);
}

void Car::FixedUpdate(float dt, int steps)
{
	if (thrust != 0)
	{
		auto dir = GetEntity()->transform.getWorldForward() * thrust;
		physics->ApplyForce(glm::vec2(dir.x, dir.z) * speed * speedMod);
	}

	if (turn != 0)
	{
		physics->ApplyAngularForce(-turn * steering);
	}
}

void Car::Notify(EventName eventName, Param * params)
{
	if (eventName == EventName::INPUT_AXIS_2D)
	{
		auto data = static_cast<TypeParam<Axis2DEvent>*>(params)->Param;

		if (data.axis == Axis::LEFT)
			turn = data.value.x;
		else if (data.axis == Axis::RIGHT)
		{
			speed += data.value.y;
		}
	}
	else if (eventName == EventName::INPUT_BUTTON)
	{
		auto data = static_cast<TypeParam<ButtonEvent>*>(params)->Param;

		if (data.button == Button::PRIMARY)
			thrust = (data.isDown) ? 1.0f : 0.0f;
	}
}

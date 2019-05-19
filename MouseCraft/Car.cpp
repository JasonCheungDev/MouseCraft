#include "Car.h"
#include <SDL2/SDL.h>
#include "UI/UIText.h"
#include <string>
#include <sstream>

Car::Car()
{
	EventManager::Subscribe(EventName::INPUT_AXIS_2D, this);
	EventManager::Subscribe(EventName::INPUT_BUTTON, this);
	EventManager::Subscribe(EventName::INPUT_KEY, this);
}

Car::~Car()
{
	EventManager::Unsubscribe(EventName::INPUT_AXIS_2D, this);
	EventManager::Unsubscribe(EventName::INPUT_BUTTON, this);
	EventManager::Unsubscribe(EventName::INPUT_KEY, this);
}

void Car::OnInitialized()
{
	physics = GetEntity()->GetComponent<PhysicsComponent>();
}

void Car::Update(float dt)
{
	auto currentSpeed = glm::length(physics->velocity);

	float percent = (currentSpeed / 20.0f);
	percent = glm::clamp(percent, 0.0f, 1.0f);
	camera->fov = (fovMax - fovMin) * percent + fovMin;
}

void Car::FixedUpdate(float dt, int steps)
{
	auto currentSpeed = glm::length(physics->velocity);

	// apply movement 

	if (thrust != 0)
	{
		auto dir = GetEntity()->transform.getWorldForward() * thrust;
		physics->ApplyForce(glm::vec2(dir.x, dir.z) * speed * speedMod);
	}

	if (turn != 0)
	{
		physics->ApplyAngularForce(-turn * steering * currentSpeed);
		wheelFL->GetEntity()->transform.setLocalRotation(glm::vec3(0, glm::radians(turn * -45.0f), 0));
		wheelFR->GetEntity()->transform.setLocalRotation(glm::vec3(0, glm::radians(turn * -45.0f), 0));
	}

	// simulate drag
	// side drag 
	auto right = GetEntity()->transform.getWorldRight();
	auto right2D = glm::vec2(right.x, right.z);
	auto sideFriction = glm::dot(physics->velocity, right2D);
	const float maxBrakeReduction = 0.75f;	// full brakes equals xx% reduction is side friction
	sideFriction = sideFriction * (1.0 - brake * maxBrakeReduction);
	physics->ApplyForce(right2D * -sideFriction);
	// back drag 
	auto forward = GetEntity()->transform.getWorldForward();
	auto forward2D = glm::vec2(forward.x, forward.z);
	float terminalSpeedPercent = brakeConverter.Convert(glm::max(0.0f, currentSpeed / maxSpeed));
	physics->ApplyForce(-forward2D * terminalSpeedPercent);

	// braking applies consistent force opposite to velocity in forward direction.
	if (brake > 0)
	{
		const float fullstopThreshold = 0.2f;
		if (currentSpeed < fullstopThreshold)
		{
			physics->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		else
		{
			const float maxBrakeSlow = 0.5f;		// full brake equals this force backwards.
			auto forwardVelocity = glm::dot(physics->velocity, forward2D);
			if (forwardVelocity > 0)
				physics->ApplyForce(-forward2D * maxBrakeSlow * brake);
			else if (forwardVelocity < 0)
				physics->ApplyForce(forward2D * maxBrakeSlow * brake);

		}
	}

	wheelFL->rotationSpeed = glm::vec3(currentSpeed, 0, 0);
	wheelFR->rotationSpeed = glm::vec3(currentSpeed, 0, 0);
	wheelBL->rotationSpeed = glm::vec3(currentSpeed, 0, 0);
	wheelBR->rotationSpeed = glm::vec3(currentSpeed, 0, 0);

	std::stringstream ss;
	ss << "SPEED: " << currentSpeed;
	speedDisplay->SetText(ss.str());
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
		else if (data.button == Button::SECONDARY)
			thrust = (data.isDown) ? -1.0f : 0.0f;
		else if (data.button == Button::AUX2)
			brake = (data.isDown) ? 1.0f : 0.0f;
	}
	else if (eventName == EventName::INPUT_KEY)
	{
		auto data = static_cast<TypeParam<KeyEvent>*>(params)->Param;

		if (!data.isDown)
			return;

		switch (data.key)
		{
		case SDLK_1:
			physics->setDrag(physics->getDrag() - 0.2f);
			break;
		case SDLK_2:
			physics->setDrag(physics->getDrag() + 0.2f);
			break;
		case SDLK_3:
			physics->setAngularDrag(physics->getAngularDrag() - 0.2f);
			break;
		case SDLK_4:
			physics->setAngularDrag(physics->getAngularDrag() + 0.2f);
			break;
		case SDLK_5:
			physics->setBounciness(physics->getBounciness() - 0.2f);
			break;
		case SDLK_6:
			physics->setBounciness(physics->getBounciness() + 0.2f);
			break;
		case SDLK_7:
			physics->setFriction(physics->getFriction() - 0.2f);
			break;
		case SDLK_8:
			physics->setFriction(physics->getFriction() + 0.2f);
			break;
		case SDLK_9:
			physics->setDensity(physics->getDensity() - 0.2f);
			break;
		case SDLK_0:
			break;
			physics->setDensity(physics->getDensity() + 0.2f);
		case SDLK_o:
			speed += 0.2f;
			break;
		case SDLK_p:
			speed -= 0.2f;
			break;
		case SDLK_k:
			steering += 0.002f;
			break;
		case SDLK_l:
			steering -= 0.002f;
			break;
		}

		std::cout << "=============================" << std::endl
			<< "Drag: " << physics->getDrag() << std::endl
			<< "Angular D: " << physics->getAngularDrag() << std::endl
			<< "Bounce: " << physics->getBounciness() << std::endl
			<< "Friction: " << physics->getFriction() << std::endl
			<< "Density: " << physics->getDensity() << std::endl
			<< "Thrust: " << speed << std::endl
			<< "Steer: " << steering << std::endl;
	}
}

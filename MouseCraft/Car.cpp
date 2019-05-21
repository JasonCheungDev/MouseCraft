#include "Car.h"
#include <SDL2/SDL.h>
#include <string>
#include "Rendering/Color.h"

Car::Car()
{
	EventManager::Subscribe(EventName::INPUT_AXIS_2D, this);
	EventManager::Subscribe(EventName::INPUT_BUTTON, this);
	EventManager::Subscribe(EventName::INPUT_KEY, this);
	// speedFormatter << std::fixed << std::setprecision(1);
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

	wheelFL->rotationSpeed = glm::vec3(currentSpeed, 0, 0);
	wheelFR->rotationSpeed = glm::vec3(currentSpeed, 0, 0);
	wheelBL->rotationSpeed = glm::vec3(currentSpeed, 0, 0);
	wheelBR->rotationSpeed = glm::vec3(currentSpeed, 0, 0);

	speedBlur->GetSettings()->SetFloat("u_Strength", percent * 2.8f);

	if (brake > 0)
	{
		brakeLightL->range = 0.5f;
		brakeLightL->intensity = 0.5f;
		brakeLightR->range = 0.5f;
		brakeLightR->intensity = 0.5f;
	}
	else
	{
		brakeLightL->range = 0.12f;
		brakeLightL->intensity = 0.25f;
		brakeLightR->range = 0.12f;
		brakeLightR->intensity = 0.25f;
	}

	count += dt;

	// UI 
	speedFormatter.str("");
	speedFormatter << "SPEED: " << (int)(currentSpeed * 10.0f);
	speedDisplay->SetText(speedFormatter.str());
	boostDisplay->size.x = boostGauge;

	//auto speedmods = accelerationMod;
	//if (speedmods > 1.0f)
	//	desiredVignetteColor = glm::vec4(FAST_COLOR.r, FAST_COLOR.g, FAST_COLOR.b, speedmods - 1.0f);
	//else
	//	desiredVignetteColor = glm::vec4(SLOW_COLOR.r, SLOW_COLOR.g, SLOW_COLOR.b, 1.0f - speedmods);
	//auto color = glm::lerp(vignette->color.vec4(), desiredVignetteColor, dt);
	//vignette->color = Color(color.r, color.g, color.b, color.a);
}

void Car::FixedUpdate(float dt, int steps)
{
	// calculate shared info
	auto currentSpeed = glm::length(physics->velocity);
	auto forward2D = GetEntity()->transform.getWorldForward2D();
	auto right2D = GetEntity()->transform.getWorldRight2D();
	float movingForward = glm::sign(glm::dot(forward2D, physics->velocity));	// if car is moving forwards or backwards

	// INPUT 

	if (thrust != 0)
	{
		physics->ApplyForce(forward2D * acceleration * accelerationMod * thrust);
	}

	if (turn != 0)
	{
		physics->ApplyAngularForce(-movingForward * turn * steering * (brake + 1.0f) * currentSpeed);
		wheelFL->GetEntity()->transform.setLocalRotation2D(glm::radians(turn * -45.0f));
		wheelFR->GetEntity()->transform.setLocalRotation2D(glm::radians(turn * -45.0f));
	}

	if (boost != 0 && boostGauge >= boostDrain * dt)
	{
		physics->ApplyForce(forward2D * boostForce * boost);
		boostGauge -= boostDrain * dt;
	}
	else
	{
		boostGauge += boostRegen * dt;
		if (boostGauge > 1.0f) boostGauge = 1.0f;
	}

	// DRAG 

	// side drag 
	auto sideFriction = glm::dot(physics->velocity, right2D);
	const float maxBrakeReduction = 0.75f;	// full brakes equals xx% reduction is side friction
	sideFriction = sideFriction * (1.0 - brake * maxBrakeReduction);
	physics->ApplyForce(right2D * -sideFriction);

	// back drag 
	float terminalSpeedPercent = brakeConverter.Convert(currentSpeed / (maxSpeed * maxSpeedMod));
	physics->ApplyForce(-movingForward * forward2D * terminalSpeedPercent);

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
			// reduce up to half of braking force if player is accelerating
			if (forwardVelocity > 0)
				physics->ApplyForce(-forward2D * maxBrakeSlow * brake * (1.0f - 0.5f * thrust));
			else if (forwardVelocity < 0)
				physics->ApplyForce(forward2D * maxBrakeSlow * brake * (1.0f - 0.5f * thrust));
		}
	}
}

void Car::Notify(EventName eventName, Param * params)
{
	if (eventName == EventName::INPUT_AXIS_2D)
	{
		auto data = static_cast<TypeParam<Axis2DEvent>*>(params)->Param;

		if (data.axis == Axis::STICK_LEFT)
			turn = data.value.x;
		else if (data.axis == Axis::STICK_RIGHT)
		{
			acceleration += data.value.y;
		}
	}
	else if (eventName == EventName::INPUT_BUTTON)
	{
		auto data = static_cast<TypeParam<ButtonEvent>*>(params)->Param;

		if (data.button == Button::SOUTH)
			thrust = (data.isDown) ? 1.0f : 0.0f;
		else if (data.button == Button::NORTH)
			thrust = (data.isDown) ? -1.0f : 0.0f;
		else if (data.button == Button::WEST)
			brake = (data.isDown) ? 1.0f : 0.0f;
		else if (data.button == Button::PRIMARY)
			boost = (data.isDown) ? 1.0f : 0.0f;
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
			acceleration += 0.2f;
			break;
		case SDLK_p:
			acceleration -= 0.2f;
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
			<< "Thrust: " << acceleration << std::endl
			<< "Steer: " << steering << std::endl;
	}
}

#pragma once

#include "Core/UpdatableComponent.h"
#include "Input/InputSystem.h"
#include "Event/EventManager.h"
#include "Physics/PhysicsComponent.h"
#include "Rendering/Camera.h"
#include "Common/Rotator.h"
#include "Common/Animation.h"
#include "UI/UIText.h"
#include "UI/UIImage.h"
#include <string>
#include <sstream>
#include "Rendering/PostProcess/PostProcess.h"
#include "Rendering/Lighting/Light.h"

class Car : public UpdatableComponent, public ISubscriber
{
public:
	Car();
	~Car();
	void OnInitialized() override;
	void Update(float dt) override;
	void FixedUpdate(float dt, int steps) override;
	virtual void Notify(EventName eventName, Param *params) override;

public:
	float acceleration = 2.0f;
	float maxSpeed = 12.0f;
	float maxSpeedMod = 1.0f;
	float steering = 0.1f;
	float accelerationMod = 1.0f;

	float boostForce = 1.0f;
	float boostGauge = 1.0f;
	float boostDrain = 0.5f;	// percent per second 
	float boostRegen = 0.25f;

	// used for fov kick
	Camera* camera;
	float fovMin = 60.0f;
	float fovMax = 105.0f;
	
	// fancy stuff 
	Rotator* wheelFL;
	Rotator* wheelFR;
	Rotator* wheelBL;
	Rotator* wheelBR;
	PostProcess* speedBlur;
	Light* brakeLightL;
	Light* brakeLightR;
	
	// UI
	UIText* speedDisplay;
	UIImage* boostDisplay;
	UIImage* vignette;

private:
	const glm::vec3 FAST_COLOR = glm::vec3(0, 0, 1);
	const glm::vec3 SLOW_COLOR = glm::vec3(1, 0, 0);
	glm::vec4 desiredVignetteColor;
	float thrust = 0.0f;
	float turn = 0.0f;
	float brake = 0.0f;
	float boost = 0.0f;
	PhysicsComponent* physics;
	X3Converter brakeConverter;
	std::stringstream speedFormatter;
	float count = 0;
};


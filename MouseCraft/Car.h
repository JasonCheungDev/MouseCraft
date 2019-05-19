#pragma once

#include "Core/UpdatableComponent.h"
#include "Input/InputSystem.h"
#include "Event/EventManager.h"
#include "Physics/PhysicsComponent.h"
#include "Rendering/Camera.h"
#include "Common/Rotator.h"

class UIText;

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
	float speed = 2.0f;
	float steering = 0.1f;
	float speedMod = 1.0f;
	// used for fov kick
	Camera* camera;
	float fovMin = 60.0f;
	float fovMax = 90.0f;
	// fancy stuff 
	Rotator* wheelFL;
	Rotator* wheelFR;
	Rotator* wheelBL;
	Rotator* wheelBR;
	// UI
	UIText* speedDisplay;

private:
	float thrust = 0.0f;
	float turn = 0.0f;
	PhysicsComponent* physics;
};


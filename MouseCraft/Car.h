#pragma once

#include "Core/UpdatableComponent.h"
#include "Input/InputSystem.h"
#include "Event/EventManager.h"
#include "Physics/PhysicsComponent.h"

class Car : public UpdatableComponent, public ISubscriber
{
public:
	Car();
	~Car();
	void OnInitialized() override;
	void FixedUpdate(float dt, int steps) override;
	virtual void Notify(EventName eventName, Param *params) override;

public:
	float speed = 5.0f;
	float steering = 5.0f;
	float speedMod = 1.0f;
private:
	float thrust = 0.0f;
	float turn = 0.0f;
	PhysicsComponent* physics;
};


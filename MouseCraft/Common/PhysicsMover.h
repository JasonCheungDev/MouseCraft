#pragma once

#include "../Core/UpdatableComponent.h"
#include "../Input/InputSystem.h"
#include "../Event/EventManager.h"
#include "../Physics/PhysicsComponent.h"

class PhysicsMover : public UpdatableComponent, public ISubscriber
{
public:
	PhysicsMover();
	~PhysicsMover();
	void OnInitialized() override;
	void FixedUpdate(float dt, int steps) override;
	virtual void Notify(EventName eventName, Param *params) override;

public:
	float speed = 5.0f;
private:
	glm::vec2 moveInput;
	glm::vec2 turnInput;
	PhysicsComponent* physics;
};


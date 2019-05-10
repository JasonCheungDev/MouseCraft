#pragma once

#include "../Core/UpdatableComponent.h"
#include "../Event/ISubscriber.h"
#include "../Event/EventManager.h"
#include <glm/glm.hpp>

class FreeLookMovement : public UpdatableComponent, public ISubscriber
{
public:
	FreeLookMovement();
	~FreeLookMovement();
	virtual void FixedUpdate(float deltaTime, int steps) override;
	float moveSpeed = 2.5f;
	float turnSpeed = 1.0f;
	void Notify(EventName eventName, Param* params) override;
private:
	glm::vec2 moveInput;
	glm::vec2 turnInput;
};


#pragma once

#include "Component.h"
#include "../Event/ISubscriber.h"

class UpdatableComponent : public Component, public ISubscriber
{
public:
	UpdatableComponent();
	~UpdatableComponent();
	virtual void Update(float deltaTime) {};
	virtual void FixedUpdate(float deltaTime, int steps) {};
	virtual void Notify(EventName eventName, Param *params);
};


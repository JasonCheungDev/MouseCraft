#pragma once

#include "Core/UpdatableComponent.h"
#include "Core/Entity.h"

class PositionMatcher : public UpdatableComponent
{
public:
	PositionMatcher();
	~PositionMatcher();
	
	void FixedUpdate(float dt, int steps) override;

public:
	Entity* target;
	float speed = 1.0f;
};


#pragma once

#include "Core/UpdatableComponent.h"
#include "Core/Entity.h"

class OrientationMatcher : public UpdatableComponent
{
public:
	OrientationMatcher();
	~OrientationMatcher();

	void FixedUpdate(float dt, int steps) override;

public:
	Entity* target;
	float speed = 1.0f;
};


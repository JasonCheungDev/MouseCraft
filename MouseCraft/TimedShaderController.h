#pragma once

#include "Core/UpdatableComponent.h"

class TimedShaderController : public UpdatableComponent
{
public:
	virtual void Update(float deltaTime) override;

private:
	float time;
};


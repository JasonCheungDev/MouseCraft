#pragma once

#include "System.h"

class UpdateSystem : public System
{
public:
	UpdateSystem();
	~UpdateSystem();
	void Update(float dt);
	void FixedUpdate(float dt, int steps);
};


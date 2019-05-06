#pragma once

#include "ExampleComponent.h"
#include "../Component.h"
#include "glm/glm.hpp"

class ExampleDerivedComponent : public ExampleComponent
{
public:
	ExampleDerivedComponent() {};
	~ExampleDerivedComponent() {};
	virtual void foobar() override {}
};


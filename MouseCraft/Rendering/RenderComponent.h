#pragma once

#include <vector>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "../Core/Entity.h"
#include "../Core/Component.h"
#include "Renderable.h"

class RenderComponent : public Component
{
// variables 
public:
	std::vector<std::shared_ptr<Renderable>> renderables; 

// functions 
public:
	void addRenderable(std::shared_ptr<Renderable> r)
	{
		renderables.push_back(r);
	}
};


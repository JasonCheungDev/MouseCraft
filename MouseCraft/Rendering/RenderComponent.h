#pragma once

#include <vector>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Renderable.h"
#include "../Core/Entity.h"
#include "../Core/Component.h"
#include "../Loading/PrefabLoader.h"
#include <json.hpp>
using json = nlohmann::json;

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


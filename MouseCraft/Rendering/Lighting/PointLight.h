#pragma once

#include <iostream>
#include "../../GL/glad.h"
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Light.h"
#include "../../Core/Entity.h"
#include "../../Loading/PrefabLoader.h"

#include <json.hpp>
using json = nlohmann::json;

class PointLight : public Light
{
public:
	// currently no shader mapping support 

	virtual void LoadUniforms(Shader* shader) override
	{
	}

	virtual void PrepareShadowmap(Shader* shader) override
	{
	}

	virtual void CleanupShadowmap(Shader* shader) override
	{
	}

	glm::mat4 getLightSpaceMatrix() override
	{
		glm::mat4 projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 50.0f);
		//glm::mat4 view = glm::lookAt(GetEntity()->position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 view = GetEntity()->transform.getWorldTransformation();
		return projection * glm::inverse(view);
	}

private:
	/* TEMPLATE
	{
		"type": "PointLight",
		"color": [1.0, 1.0, 1.0],
		"intensity": 1.0,
		"range": 5.0,
	}
	*/
	static Component* Create(json json);
	static ComponentRegistrar reg;
};


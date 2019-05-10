#pragma once

#include <iostream>
#include "../../GL/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Light.h"
#include "../../Core/Entity.h"
#include "../../Loading/PrefabLoader.h"

#include <json.hpp>
using json = nlohmann::json;

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	~DirectionalLight();

	// applies additional lighting to entire scene (even when light wouldn't hit it).
	float ambientIntensity = 0.0f;

	virtual void LoadUniforms(Shader* shader) override {}

	virtual void PrepareShadowmap(Shader* shader) override;

	// WARNING: don't call this 
	virtual void CleanupShadowmap(Shader* shader) override;

	glm::mat4 getLightSpaceMatrix() override;

private:
	/* TEMPLATE
	{
		"type": "DirectionalLight",
		"color": [1.0, 1.0, 1.0],
		"intensity": 1.0,
		"ambient": 0.0,
		// OPTIONAL
		"shadow": {
			"near": 0.1,
			"far": 40.0,
			"fov": 20.0
		}
	}
	*/
	static Component* Create(json json);
	static ComponentRegistrar reg;

};


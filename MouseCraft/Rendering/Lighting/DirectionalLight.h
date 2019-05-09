#pragma once

#include <iostream>
#include "../../GL/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Light.h"
#include "../../Core/Entity.h"

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
};


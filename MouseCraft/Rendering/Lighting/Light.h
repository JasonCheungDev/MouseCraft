#pragma once

#include <typeinfo>
#include <typeindex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Shader.h"
#include "../../Core/Entity.h"


// Use this struct for simple light handling 
const int MAX_LIGHTS = 32;
struct LightSimple
{
	glm::vec3 Position;
	glm::vec3 Color;
};

// Generic Light class for Point and Directional lights. 
class Light : public Component
{
public:
	glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
	float intensity = 1.0;
	float range = 5.0;
	float shadowMapNear = 0.1f;		// shadow near plane
	float shadowMapFar = 40.0f;		// shadow far plane
	float shadowFov = 20.0f;		// shadow field of view (perspective angles or othro pixels).

public:
	virtual void LoadUniforms(Shader* shader)	  = 0;
	virtual void PrepareShadowmap(Shader* shader) = 0;
	virtual void CleanupShadowmap(Shader* shader) = 0;
	virtual glm::mat4 getLightSpaceMatrix() { return glm::mat4(1.0f); };
	GLuint GetShadowMapID() const { return TexId; }

protected:
	GLuint FBO;		// Shadow map frame buffer
	GLuint TexId;	// Shadow map texture ID 
};


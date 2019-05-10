#include "DirectionalLight.h"

#include "../Constants.h"
#include "../../Core/ComponentFactory.h"

constexpr unsigned int SHADOWMAP_RESOLUTION = 2048;	// use a high resolution as directional lights encompass a large area

DirectionalLight::DirectionalLight() : Light()
{
	// Generate shadow map 
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &TexId);
	glBindTexture(GL_TEXTURE_2D, TexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);	// make out of border unshadowed 

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TexId, 0);

	glDrawBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "ERROR: DirectionalLight failed to create shadow map" << std::endl;
	}
}

DirectionalLight::~DirectionalLight() {}

// TODO: TONS OF MAGIC NUMBERS
void DirectionalLight::PrepareShadowmap(Shader * shader)
{
	glViewport(0, 0, SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void DirectionalLight::CleanupShadowmap(Shader * shader)
{
	std::cerr << "I SAID DONT CALL CLEANUPSHADOWMAP" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 640, 480);
}

glm::mat4 DirectionalLight::getLightSpaceMatrix()
{
	glm::mat4 projection = glm::ortho(-shadowFov, shadowFov, -shadowFov, shadowFov, shadowMapNear, shadowMapFar);
	//glm::mat4 view = glm::lookAt(GetEntity()->position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 view = GetEntity()->transform.getWorldTransformation();
	return projection * glm::inverse(view);
}

ComponentRegistrar DirectionalLight::reg("DirectionalLight", &DirectionalLight::Create);

Component * DirectionalLight::Create(json json)
{
	auto c = ComponentFactory::Create<DirectionalLight>();
	c->color = glm::vec3(json["color"][0].get<float>(), json["color"][1].get<float>(), json["color"][2].get<float>());
	c->intensity = json["intensity"].get<float>();
	c->ambientIntensity = json["ambient"].get<float>();

	if (json.find("shadow") != json.end())
	{
		c->shadowMapNear = json["shadow"]["near"].get<float>();
		c->shadowMapFar = json["shadow"]["far"].get<float>();
		c->shadowFov = json["shadow"]["fov"].get<float>();
	}

	return c;
}

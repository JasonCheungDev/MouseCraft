#include "AmbientLight.h"

#include "../../Core/ComponentFactory.h"

AmbientLight::AmbientLight()
{
}

AmbientLight::~AmbientLight()
{
}

ComponentRegistrar AmbientLight::reg("AmbientLight", &AmbientLight::Create);

Component * AmbientLight::Create(json json)
{
	auto c = ComponentFactory::Create<AmbientLight>();
	c->color = glm::vec3(json["color"][0].get<float>(), json["color"][1].get<float>(), json["color"][2].get<float>());
	c->intensity = json["intensity"].get<float>();
	return c;
}
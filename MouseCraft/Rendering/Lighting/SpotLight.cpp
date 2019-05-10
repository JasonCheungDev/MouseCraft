#include "SpotLight.h"

#include "../../Core/ComponentFactory.h"

SpotLight::SpotLight()
{
}

SpotLight::~SpotLight()
{
}

ComponentRegistrar SpotLight::reg("SpotLight", &SpotLight::Create);

Component * SpotLight::Create(json json)
{
	auto c = ComponentFactory::Create<SpotLight>();
	c->color = glm::vec3(json["color"][0].get<float>(), json["color"][1].get<float>(), json["color"][2].get<float>());
	c->intensity = json["intensity"].get<float>();
	c->range = json["range"].get<float>();
	c->angle = json["angle"].get<float>();
	return c;
}
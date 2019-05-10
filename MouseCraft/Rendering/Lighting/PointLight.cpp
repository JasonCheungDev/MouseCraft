#include "PointLight.h"

#include "../../Core/ComponentFactory.h"

ComponentRegistrar PointLight::reg("PointLight", &PointLight::Create);

Component* PointLight::Create(json json)
{
	auto c = ComponentFactory::Create<PointLight>();
	c->color = glm::vec3(json["color"][0].get<float>(), json["color"][1].get<float>(), json["color"][2].get<float>());
	c->intensity = json["intensity"].get<float>();
	c->range = json["range"].get<float>();
	return c;
}

#include "JsonGL.h"

glm::vec3 jsonToVec3(const nlohmann::json & json)
{
	return glm::vec3(json[0].get<float>(), json[1].get<float>(), json[2].get<float>());
}

glm::vec4 jsonToVec4(const nlohmann::json & json)
{
	return glm::vec4(json[0].get<float>(), json[1].get<float>(), json[2].get<float>(), json[4].get<float>());
}

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <json.hpp>

glm::vec3 jsonToVec3(const nlohmann::json& json);

glm::vec4 jsonToVec4(const nlohmann::json& json);
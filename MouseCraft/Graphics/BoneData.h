#pragma once

#include <glm/glm.hpp>
#include <string>
#include "../Core/Entity.h"

class BoneData
{
public:
	BoneData();
	~BoneData();
	std::string name;
	int id;
	glm::mat4 offsetMatrix;
	Entity* transform;
};

// Layout of a boned Vertex. Only used to determine the size of the Vertex.
struct BonedVertex
{
	glm::vec3 pos;
	glm::vec3 nrm;
	glm::vec2 uv;
	glm::ivec4 bones;
	glm::vec4 weights;
};
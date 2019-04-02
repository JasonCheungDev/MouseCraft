#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct BoneAnim
{
	std::string name;	// bone name
	std::vector<std::pair<float, glm::vec3>> positions;
	std::vector<std::pair<float, glm::quat>> rotations;
	std::vector<std::pair<float, glm::vec3>> scales;
};

struct Animation
{
	std::string name;	// animation name 
	float duration;
	float ticksPerSecond;
	std::vector<BoneAnim> frames;
};
#pragma once

#include <glm/vec2.hpp>

struct Rect
{
	float left;
	float top;
	float right;
	float bottom;

	float getWidth() { return abs(right - left); }
	float getHeight() { return abs(top - bottom); }
	glm::vec2 getSize() { return glm::vec2(getWidth(), getHeight()); }
	float getXCenter() { return (left + right) / 2; }
	float getYCenter() { return (bottom + top) / 2; }
	glm::vec2 getCenter() { return glm::vec2(getXCenter(), getYCenter()); }
};
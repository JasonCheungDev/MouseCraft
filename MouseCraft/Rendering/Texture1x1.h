#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Texture.h"

// Convenience class to create a 1x1 texture with given color/value. 
// Mainly used for default textures.
class Texture1x1 : public Texture
{
public:
	Texture1x1(float value);
	Texture1x1(glm::vec3 value);
	Texture1x1(glm::vec4 value);
};
#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Graphics/Image.h"

class CubeMap
{
public:
	// Creates a cubemap texture on the GPU.
	// Order: right,left,top,bottom,front,back
	// Image data can be deallocated afterwards.
	CubeMap(std::vector<Image*> images);

	// Deallocates texture from GPU as well.
	~CubeMap();

	// Texture ID on the GPU. 
	unsigned int GetId() { return _id; };

private:
	unsigned int _id;
};


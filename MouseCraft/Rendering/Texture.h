#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Image.h"

class Texture
{
public:
	// Creates a texture on the GPU.
	// Image data can be deallocated afterwards.
	Texture(Image* image);

	// Creates a texture on the GPU.
	// Image data can be deallcoated afterwards.
	Texture(unsigned char* data, int width, int height, int channels = 4);
	
	// Deallocates texture from GPU as well.
	~Texture();

	// Width and height of texture in pixels.
	glm::ivec2 GetSize() { return _size; };

	// Number of channels (R, RGB, RGBA).
	unsigned int GetChannels() { return _channels; };

	// Texture ID on the GPU. 
	unsigned int GetId() { return _id; };

private:
	glm::ivec2 _size;
	unsigned int _channels;
	unsigned int _id;
};

// simple wrapper to associate a texture with a shader's uniform
struct TextureShaderInfo
{
	std::string uniform;
	Texture* texture;
};
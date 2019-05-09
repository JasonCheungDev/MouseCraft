#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../Rendering/Texture.h"
#include "../Rendering/CubeMap.h"

class TextureLoader
{
public:
	// Loads a texture directly into OpenGL.
	// Will return a preloaded texture if found.
	// If you want the image data use ImageLoader instead. 
	static Texture* Load(std::string path);

	// Loads a texture directly into OpenGL.
	// Recommended to use Load() instead. 
	static Texture* LoadCopy(std::string path);

	// Loads a cubemap texture into OpenGl with 6 specified paths to images. 
	// Will return a preloaded texture if found.
	// Order: right,left,top,bottom,front,back
	static CubeMap* LoadCubeMap(std::vector<std::string> facesPath);


	// Loads a cubemap texture into OpenGl with 6 specified paths to images. 
	// Recommended to use LoadCubeMap() instead.
	// Order: right,left,top,bottom,front,back
	static CubeMap* LoadCubeMapCopy(std::vector<std::string> facesPath);
};


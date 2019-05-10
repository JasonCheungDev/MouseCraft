#include "TextureLoader.h"

#include <numeric>
#include <iostream>
#include <stb_image.h>
#include "ImageLoader.h"
#include "../GL/glad.h"
#include "ResourceCache.h"

Texture* TextureLoader::Load(std::string path)
{
	std::cout << "Loading texture name: " << path << std::endl;

	// Try to find preloaded texture 
	Texture* texture;
	texture = ResourceCache<Texture>::Instance().Get(path);
	if (texture)
		return texture;

	// Try to find preloaded image data 
	Image* img = ResourceCache<Image>::Instance().Get(path);

	if (img)
	{
		// Found image data 
		texture = new Texture(img);
	}
	else
	{
		// Did not find image data
		img = ImageLoader::loadImage(path);
		texture = new Texture(img);
		delete(img);
	}

	// Add to cache for future 
	if (texture)
	{
		ResourceCache<Texture>::Instance().Add(path, texture);
	}

	return texture;
}

Texture* TextureLoader::LoadCopy(std::string path)
{
	Texture* texture;
	
	Image* img = ResourceCache<Image>::Instance().Get(path);
	if (img)
	{
		// Found image data 
		texture = new Texture(img);
	}
	else
	{
		// Did not find image data
		img = ImageLoader::loadImage(path);
		texture = new Texture(img);
		delete(img);
	}

	return texture;
}

CubeMap* TextureLoader::LoadCubeMap(std::vector<std::string> facesPath)
{
	CubeMap* cubemap;
	
	// Try to find preloaded cubemap
	std::string key = std::accumulate(facesPath.begin(), facesPath.end(), std::string(""));
	cubemap = ResourceCache<CubeMap>::Instance().Get(key);
	if (cubemap)
		return cubemap;

	// Create cubemap otherwise
	std::vector<Image*> images;
	for (unsigned int i = 0; i < 6; i++)
		images.push_back(ImageLoader::loadImage(facesPath[i]));

	cubemap = new CubeMap(images);

	for (auto i : images)
		delete(i);

	// Add to cache for future
	if (cubemap)
		ResourceCache<CubeMap>::Instance().Add(key, cubemap);

	return cubemap;
}

CubeMap* TextureLoader::LoadCubeMapCopy(std::vector<std::string> facesPath)
{
	std::vector<Image*> images;
	for (unsigned int i = 0; i < 6; i++)
		images.push_back(ImageLoader::loadImage(facesPath[i]));

	CubeMap* cubemap = new CubeMap(images);

	for (auto i : images)
		delete(i);

	return cubemap;
}

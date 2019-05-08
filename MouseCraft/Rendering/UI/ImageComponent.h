#pragma once

#include <string>
#include <glm\glm.hpp>
#include "..\..\Core\Entity.h"
#include "..\..\Core\Component.h"
#include "View.h"

class ImageComponent : public Component, public View
{
public:
	ImageComponent();
	~ImageComponent();

	// load the image to be used for this component
	void loadImage(std::string path);

	glm::vec2 getPosition()
	{
		return GetEntity()->transform.getWorldPosition();
	}

	unsigned int imageId = -1;
	int width;	
	int height;	
	float opacity = 1.0f; 
	glm::vec3 tint = glm::vec3(1,1,1);
};


#include "ImageComponent.h"

ImageComponent::ImageComponent() : Component()
{
	alignment = TextAlignment::Center;
}


ImageComponent::~ImageComponent()
{
}

void ImageComponent::loadImage(std::string path)
{
	// TODO:
	// imageId = Game::instance().loader.LoadTexture(path, &this->width, &this->height);
}

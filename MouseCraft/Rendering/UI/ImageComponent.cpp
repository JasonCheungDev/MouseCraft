#include "ImageComponent.h"
#include "../../Loading/TextureLoader.h"

ImageComponent::ImageComponent() : Component()
{
	alignment = TextAlignment::Center;
}


ImageComponent::~ImageComponent()
{
}

void ImageComponent::loadImage(std::string path)
{
	auto tex = TextureLoader::Load(path);
	imageId = tex->GetId();
	width = tex->GetSize().x;
	height = tex->GetSize().y;
}

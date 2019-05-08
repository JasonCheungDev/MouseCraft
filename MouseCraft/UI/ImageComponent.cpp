#include "ImageComponent.h"
#include "../Texture.h"
#include "../Loading/TextureLoader.h"

ImgComponent::ImgComponent(std::string imagePath, float width, float height, float x, float y) :
    UIComponent(width, height, x, y), _imagePath(imagePath) 
{
    //aspectRatio = float(texture.width) / texture.height;
    // color = Color(1, 1, 1);
	Texture* texture = TextureLoader::Load(imagePath);
	textureId = texture->GetId();
}

bool ImgComponent::IsTransparent() { return true; }

std::string ImgComponent::GetImagePath() {
	return _imagePath;
}

void ImgComponent::SetImagePath(std::string path) {
	_imagePath = path;
}

void ImgComponent::setupModels() {
	UIComponent::setupModels();
	//models[0]->setTexture(new std::string(_imagePath));
}
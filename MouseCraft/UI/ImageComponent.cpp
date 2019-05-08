#include "ImageComponent.h"
#include "../Texture.h"
#include "../Loading/TextureLoader.h"

ImgComponent::ImgComponent(std::string imagePath, float width, float height, float x, float y) :
    UIComponent(width, height, x, y)
{
	texture = TextureLoader::Load(imagePath);
    //aspectRatio = float(texture.width) / texture.height;
    //color = Color(1, 1, 1);
}

bool ImgComponent::IsTransparent() 
{ 
	return true; 
}

Texture* ImgComponent::GetTexture() const
{
	return texture;
}

void ImgComponent::LoadTexture(std::string path) 
{
	texture = TextureLoader::Load(path);
}

void ImgComponent::SetTexture(Texture * texture)
{
	this->texture = texture;
}

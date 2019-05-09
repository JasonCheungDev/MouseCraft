#include "UIImage.h"
#include "../Rendering/Texture.h"
#include "../Loading/TextureLoader.h"

UIImage::UIImage(std::string imagePath, float width, float height, float x, float y) :
    UIComponent(width, height, x, y)
{
	texture = TextureLoader::Load(imagePath);
    //aspectRatio = float(texture.width) / texture.height;
    //color = Color(1, 1, 1);
}

bool UIImage::IsTransparent() 
{ 
	return true; 
}

Texture* UIImage::GetTexture() const
{
	return texture;
}

void UIImage::LoadTexture(std::string path) 
{
	texture = TextureLoader::Load(path);
}

void UIImage::SetTexture(Texture * texture)
{
	this->texture = texture;
}

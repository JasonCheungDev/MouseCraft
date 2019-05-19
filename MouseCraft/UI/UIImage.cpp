#include "UIImage.h"
#include "../Rendering/Texture.h"
#include "../Loading/TextureLoader.h"
#include "../Core/ComponentFactory.h"

UIImage::UIImage(const std::string imagePath, float xOffset, float yOffset) 
	: texture(TextureLoader::Load(imagePath)), 
	UIComponent(texture->GetSize().x, texture->GetSize().y, xOffset, yOffset)
{
	widthType = UNIT_PIXEL;
	heightType = UNIT_PIXEL;
}

UIImage::UIImage(std::string imagePath, float width, float height, float x, float y) :
    UIComponent(width, height, x, y)
{
	texture = TextureLoader::Load(imagePath);
    //aspectRatio = float(texture.width) / texture.height;
    //color = Color(1, 1, 1);
}

UIImage::UIImage(Texture * texture, float width, float height, float x, float y) 
	: texture(texture), UIComponent(width, height, x, y)
{
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

Component* UIImage::Create(nlohmann::json json)
{
	auto img = ComponentFactory::Create<UIImage>(nullptr, 0, 0, 0, 0);
	
	img->InitalizeFromJson(json);

	img->LoadTexture(json["path"].get<std::string>());

	return img;
}

ComponentRegistrar UIImage::reg("Image", &UIImage::Create);
#pragma once

#include "UIComponent.h"
#include <string>

/**
Type of UIComponent that displays an image file
*/
class ImgComponent : public UIComponent {
public:
	ImgComponent(std::string imagePath, float width, float height, float x, float y);
	void setupModels() override;
    bool IsTransparent();
	std::string GetImagePath();
	void SetImagePath(std::string path);
	unsigned int textureId;
private:
    std::string _imagePath;
};
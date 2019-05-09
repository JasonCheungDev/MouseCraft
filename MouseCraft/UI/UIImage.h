#pragma once

#include "UIComponent.h"
#include <string>

class Texture;

/**
Type of UIComponent that displays an image file
*/
class UIImage : public UIComponent {
public:
	UIImage(std::string imagePath, float width, float height, float x, float y);
    
	bool IsTransparent() override;
	
	Texture* GetTexture() const;

	void LoadTexture(std::string path);

	void SetTexture(Texture* texture);

private:
	Texture* texture;
};
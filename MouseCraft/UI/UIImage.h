#pragma once

#include "UIComponent.h"
#include <string>
#include "../Loading/PrefabLoader.h"
#include <json.hpp>
using json = nlohmann::json;
using namespace std;

class Texture;

/**
Type of UIComponent that displays an image file
*/
class UIImage : public UIComponent {
public:
	
	// Create an image display with given path with the same size as the loaded image. 
	UIImage(const std::string imagePath, float xOffset, float yOffset);

	// Create an image display with given path and transform.
	// Width and height are percentages by default.
	UIImage(std::string imagePath, float width, float height, float x, float y);
    
	// Create an image display with given texture.
	UIImage(Texture* texture, float width, float height, float x, float y);

	bool IsTransparent() override;
	
	Texture* GetTexture() const;

	void LoadTexture(std::string path);

	void SetTexture(Texture* texture);

private:
	Texture* texture;

	static Component* Create(json json);
	static ComponentRegistrar reg; 
};
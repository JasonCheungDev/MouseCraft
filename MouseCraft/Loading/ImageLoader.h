#pragma once
#include "../Rendering/Image.h"
#include <string>

class ImageLoader {
public:
	static Image* loadImage(std::string filename);
};
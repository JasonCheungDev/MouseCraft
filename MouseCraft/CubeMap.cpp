#include "CubeMap.h"


CubeMap::CubeMap(std::vector<Image*> images)
{
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

	for (unsigned int i = 0; i < images.size(); i++)
	{
		// determine format 
		GLenum format;
		if (images[i]->getChannels() == 1)
			format = GL_RED;
		else if (images[i]->getChannels() == 3)
			format = GL_RGB;
		else if (images[i]->getChannels() == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, images[i]->getWidth(), images[i]->getHeight(), 0, format, GL_UNSIGNED_BYTE, images[i]->getData()
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

CubeMap::~CubeMap()
{
	glDeleteTextures(1, &_id);
}

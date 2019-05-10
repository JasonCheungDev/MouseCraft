#include "Texture.h"


Texture::Texture(Image * image) : 
	Texture(image->getData(), image->getWidth(), image->getHeight(), image->getChannels())
{
}

Texture::Texture(unsigned char* data, int width, int height, int channels)
{
	// determine format 
	GLenum format;
	if (channels == 1)
		format = GL_RED;
	else if (channels == 3)
		format = GL_RGB;
	else if (channels == 4)
		format = GL_RGBA;

	// generate opengl texture
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);

	// load data 
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// set parameters 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	_size = { width, height };
	_channels = channels;
}

Texture::~Texture()
{
	glDeleteTextures(1, &_id);
}


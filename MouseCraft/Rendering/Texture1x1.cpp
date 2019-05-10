#include "Texture1x1.h"

Texture1x1::Texture1x1(float value)
{
	// generate opengl texture
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);

	// load data 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_FLOAT, &value);
	glGenerateMipmap(GL_TEXTURE_2D);

	// set parameters 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	_size = { 1, 1 };
	_channels = 1;
}

Texture1x1::Texture1x1(glm::vec3 value)
{
	// generate opengl texture
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);

	// load data 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, &value);
	glGenerateMipmap(GL_TEXTURE_2D);

	// set parameters 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	_size = { 1, 1 };
	_channels = 3;
}

Texture1x1::Texture1x1(glm::vec4 value)
{
	// generate opengl texture
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);

	// load data 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, &value);
	glGenerateMipmap(GL_TEXTURE_2D);

	// set parameters 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	_size = { 1, 1 };
	_channels = 4;
}
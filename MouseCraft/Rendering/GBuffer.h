#pragma once

#include "Shader.h"

// FrameBuffer used for main deferred rendering textures.
class GBuffer
{
// functions
public:
	GBuffer();
	~GBuffer();
	unsigned int Read(Shader* shader);
	void Draw(bool clear = true);

private:
	void GenerateFrameBuffer(unsigned int width, unsigned int height);

// variables 
private:
	unsigned int _fbo;
	unsigned int _posTex, _nrmTex, _colTex, _dphTex;
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
};


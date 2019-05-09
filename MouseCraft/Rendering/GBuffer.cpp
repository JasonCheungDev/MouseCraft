#include "GBuffer.h"

#include <iostream>
#include "Constants.h"
#include "../Core/OmegaEngine.h"

GBuffer::GBuffer()
{
	GenerateFrameBuffer(OmegaEngine::Instance().getWindow()->getWidth(), OmegaEngine::Instance().getWindow()->getHeight());
}

GBuffer::~GBuffer()
{
	glDeleteFramebuffers(1, &_fbo);
}

unsigned int GBuffer::Read(Shader * shader)
{
	// Set order (in case shader file mixes uniform declaration)
	shader->setInt(SHADER_FB_POS, 0);	
	shader->setInt(SHADER_FB_NRM, 1);
	shader->setInt(SHADER_FB_COL, 2);
	shader->setInt(SHADER_FB_DPH, 3);

	// bind textures for reading 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _posTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _nrmTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _colTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _dphTex);

	return 4;
}

void GBuffer::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glDrawBuffers(3, attachments);	
}

void GBuffer::GenerateFrameBuffer(unsigned int width, unsigned int height)
{
	// initialize frame buffer object 
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	// position texture 
	glGenTextures(1, &_posTex);
	glBindTexture(GL_TEXTURE_2D, _posTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _posTex, 0);
	// normal texture 
	glGenTextures(1, &_nrmTex);
	glBindTexture(GL_TEXTURE_2D, _nrmTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _nrmTex, 0);
	// color (albedo) texture
	glGenTextures(1, &_colTex);
	glBindTexture(GL_TEXTURE_2D, _colTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _colTex, 0);
	// depth texture (create as texture so we can read from it) 
	glGenTextures(1, &_dphTex);
	glBindTexture(GL_TEXTURE_2D, _dphTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _dphTex, 0);

	// check valid
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "OPENGL: Failed to create GBuffer. GL ERROR: " << glGetError() << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#include "FrameBuffer.h"

#include "../Core/OmegaEngine.h"

FrameBuffer::FrameBuffer()
{
	GenerateFrameBuffer(OmegaEngine::Instance().getWindow()->getWidth(), OmegaEngine::Instance().getWindow()->getHeight());
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
{
	GenerateFrameBuffer(width, height);
}


FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &_fbo);
	glDeleteTextures(1, &_tex);
}

unsigned int FrameBuffer::Read(unsigned int textureSlot)
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, _tex);
	return 1;
}

void FrameBuffer::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

unsigned int FrameBuffer::GetTexId()
{
	return _tex;
}

void FrameBuffer::GenerateFrameBuffer(unsigned int width, unsigned int height)
{
	// initialize frame buffer object 
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	// create color texture
	glGenTextures(1, &_tex);
	glBindTexture(GL_TEXTURE_2D, _tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex, 0);
	// create depth buffer 
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// check valid
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "OPENGL: Failed to create framebuffer. GL ERROR: " << glGetError() << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#include "renderTexture.h"


RenderTex::RenderTex()
{
	glGenTextures(1, &ID);
	glGenFramebuffers(1, &frameBuffer);
	glGenRenderbuffers(1, &renderBuffer);
}

RenderTex::~RenderTex()
{
}

unsigned int RenderTex::GetFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer failed" << std::endl;

	return frameBuffer;
}

void RenderTex::setViewPort(glm::vec4 _view)
{
	glViewport(0, 0, _view.z, _view.w);
}

void RenderTex::resetViewPort()
{
	glViewport(0, 0, windowWidth, windowHeight);
}

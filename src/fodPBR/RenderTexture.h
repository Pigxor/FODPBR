#ifndef _RENDER_TEXTURE_H_
#define _RENDER_TEXTURE_H_

#include <iostream>
#include "metaHeader.h"

class RenderTex
{
	unsigned int frameBuffer;
	unsigned int renderBuffer;
	unsigned int ID;


public:
	RenderTex();
	~RenderTex();
	unsigned int GetFrameBuffer();
	void setViewPort(glm::vec4 _view);
	void resetViewPort();
};

#endif // !

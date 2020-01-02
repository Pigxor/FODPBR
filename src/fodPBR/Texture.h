#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>
#include "metaHeader.h"

#include "stb_image.h"
#include <vector>

class Tex
{
	unsigned int texID;
	int w;
	int h;
	unsigned char* data;
	GLenum format;
	bool cubeMap;
public:
	Tex(glm::vec2 _viewport, int _mipmaps);
	Tex(const std::string _text, bool _HDR);
	Tex(const std::vector<std::string> _text);

	void loadTex(const std::string _text, bool _HDR);
	unsigned int getID();
	bool getCubeMap();

};
#endif
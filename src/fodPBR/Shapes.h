#ifndef _SHAPES_H_
#define _SHAPES_H_

#include "metaHeader.h"
#include "Texture.h"

class Shape
{
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int indexCount;
	bool sphere;

	std::vector<Tex> textures;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	std::vector<float> data;

public:
	Shape(bool _sphere);
	void render();
	void renderQuad();
	void addTex(Tex _texture);
	void bindTex();
	void unbindTex();


};

#endif

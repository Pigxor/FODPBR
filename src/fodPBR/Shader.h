#ifndef _SHADER_H_
#define _SHADER_H_
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "metaHeader.h"

class Shader
{
	void pollErrors(unsigned int _shader, std::string _type);



public:
	unsigned int progID;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void setActiveShader();
	void setUniformBool(const std::string &name, bool value) const;
	void setUniformInt(const std::string &name, int value) const;
	void setUniformFloat(const std::string &name, float value) const;
	void setUniformVec3(const std::string &name, glm::vec3 value) const;
	void setUniformMat4(const std::string &name, glm::mat4 value) const;
};



#endif // !_SHADER_H_
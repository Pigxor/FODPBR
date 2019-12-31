#include "shader.h"


void Shader::pollErrors(unsigned int _shader, std::string _type)
{
	int success;
	char infoLog[1024];
	if (_type != "prog")
	{
		glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(_shader, 1024, NULL, infoLog);
			std::cout << "Shader failed to compile - " << _type << std::endl << infoLog << std::endl << std::endl;
		}
	}
	else
	{
		glGetProgramiv(_shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(_shader, 1024, NULL, infoLog);
			std::cout << "Program Link Error - " << _type << std::endl << infoLog << std::endl << std::endl;
		}
	}
}

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Failed to read shader" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	unsigned int vertex, fragment;
	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	pollErrors(vertex, "vert");

	//fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	pollErrors(fragment, "frag");

	// shader Program
	progID = glCreateProgram();
	glAttachShader(progID, vertex);
	glAttachShader(progID, fragment);
	glLinkProgram(progID);
	pollErrors(progID, "prog");

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::setActiveShader()	
{
	glUseProgram(progID);
}

void Shader::setUniformBool(const std::string & name, bool value) const
{
	glUniform1i(glGetUniformLocation(progID, name.c_str()), (int)value);
}

void Shader::setUniformInt(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(progID, name.c_str()), value);
}

void Shader::setUniformFloat(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(progID, name.c_str()), value);
}

void Shader::setUniformVec3(const std::string & name, glm::vec3 value) const
{
	glUniform3fv(glGetUniformLocation(progID, name.c_str()), 1, &value[0]);
}

void Shader::setUniformMat4(const std::string & name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(progID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

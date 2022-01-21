#ifndef SHADER_H
#define SHADER_H

#include "GLHandles.h"

#include <GL/glew.h>

#include <string>

class ShaderProgram;

class Shader {

public:
	Shader(const std::string& path, GLenum type);

	// Public interface
	std::string getPath() const { return path; }
	GLenum getType() const { return type; }
	
	// Allow casting to GLuint
	operator GLuint() const { return shaderID; }

private:
	ShaderHandle shaderID;
	GLenum type;

	std::string path;

	bool compile();
};

#endif
#pragma once

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

	void friend attach(ShaderProgram& sp, Shader& s);

private:
	ShaderHandle shaderID;
	GLenum type;

	std::string path;

	bool compile();
};


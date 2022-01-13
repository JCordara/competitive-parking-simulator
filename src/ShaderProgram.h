#pragma once

#include "Shader.h"

#include "GLHandles.h"

#include <GL/glew.h>

#include <string>


class ShaderProgram {

public:
	ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);

	// Public interface
	bool recompile();
	void use() const { glUseProgram(programID); }

	void friend attach(ShaderProgram& sp, Shader& s);

	operator GLuint() const {
		return programID;
	}

private:
	ShaderProgramHandle programID;

	Shader vertex;
	Shader fragment;

	bool checkAndLogLinkSuccess() const;
};

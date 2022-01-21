#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Shader.h"

#include "GLHandles.h"

#include <GL/glew.h>

#include <string>
#include <list>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Log.h"


class ShaderProgram {

public:

	ShaderProgram();
	ShaderProgram(const std::string& vertexPath, 
				  const std::string& fragmentPath);

	// Public interface
	void use() const { glUseProgram(programID); }
	void addShader(GLenum type, const std::string& vertexPath);
	bool recompile();

	// Allow casting to GLuint
	operator GLuint() const { return programID; }

private:

	ShaderProgramHandle programID;

	std::list<Shader> shaders;

	bool link() const;
};


#endif
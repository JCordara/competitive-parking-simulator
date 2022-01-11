#pragma once

#include "GLHandles.h"
#include <GL/glew.h>
#include <string>

#include <glm/glm.hpp>


class Texture {
public:
	Texture(std::string path, GLint interpolation);

	// Public interface
	std::string getPath() const { return path; }
	GLenum getInterpolation() const { return interpolation; }

	// Although uint (i.e. uvec2) might make more sense here, went with int (i.e. ivec2) under
	// the assumption that most students will want to work with ints, not uints, in main.cpp
	glm::ivec2 getDimensions() const { return glm::uvec2(width, height); }

	void bind() { glBindTexture(GL_TEXTURE_2D, textureID); }
	void unbind() { glBindTexture(GL_TEXTURE_2D, textureID); }

private:
	TextureHandle textureID;
	std::string path;
	GLint interpolation;

	int width;
	int height;

};

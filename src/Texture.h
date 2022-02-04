#pragma once

#include "GLHandles.h"
#include <GL/glew.h>
#include <string>

#include <glm/glm.hpp>
#include <vector>


class Texture {
public:

	void load(std::string path, GLint interpolation);
	void setUpInternal(int width, int height, GLint interpolation, GLuint format, GLenum type);
	void setBorderColour(glm::vec4 col);

	// Although uint (i.e. uvec2) might make more sense here, went with int (i.e. ivec2) under
	// the assumption that most students will want to work with ints, not uints, in main.cpp
	std::string getPath() const { return path; }
	GLenum getInterpolation() const { return interpolation; }
	glm::ivec2 getDimensions() const { return glm::uvec2(width, height); }
	GLuint getTextureHandleID() { return textureID.value(); }

	void bind() { glBindTexture(GL_TEXTURE_2D, textureID); }
	void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

private:
	TextureHandle textureID;
	std::string path;
	GLint interpolation;
	int width;
	int height;
};

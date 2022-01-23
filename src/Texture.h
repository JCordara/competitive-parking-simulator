#pragma once

#include "GLHandles.h"
#include <GL/glew.h>
#include <string>

#include <glm/glm.hpp>
#include <vector>


class Texture {
public:
	Texture() : textureID() {}
	Texture(std::string path, GLint interpolation);
	Texture(int width, int height, GLint interpolation, GLuint format, GLenum type);

	// Public interface
	std::string getPath() const { return path; }
	GLenum getInterpolation() const { return interpolation; }

	void setBorderColour(glm::vec4 col){
		this->bind();
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &col[0]);
		this->unbind();
	}

	// Although uint (i.e. uvec2) might make more sense here, went with int (i.e. ivec2) under
	// the assumption that most students will want to work with ints, not uints, in main.cpp
	glm::ivec2 getDimensions() const { return glm::uvec2(width, height); }

	void bind() { glBindTexture(GL_TEXTURE_2D, textureID); }
	void unbind() { glBindTexture(GL_TEXTURE_2D, textureID); }
	GLuint getTextureHandleID() { return textureID; }

private:
	TextureHandle textureID;
	std::string path;
	GLint interpolation;

	int width;
	int height;

};

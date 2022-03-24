#pragma once

#include "GLHandles.h"
#include <GL/glew.h>
#include <string>

#include <glm/glm.hpp>
#include <vector>


class CubeMap {
public:

	CubeMap(std::vector<std::string> paths, GLint interpolation);

	// Although uint (i.e. uvec2) might make more sense here, went with int (i.e. ivec2) under
	// the assumption that most students will want to work with ints, not uints, in main.cpp
	std::vector<std::string> getPaths() const { return paths; }
	GLenum getInterpolation() const { return interpolation; }
	glm::ivec2 getDimensions() const { return glm::uvec2(width, height); }
	GLuint getTextureHandleID() { return textureID.value(); }

	void bind() { glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); }
	void unbind() { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }

private:
	TextureHandle textureID;
	std::vector<std::string> paths;
	GLint interpolation;
	int width;
	int height;
};

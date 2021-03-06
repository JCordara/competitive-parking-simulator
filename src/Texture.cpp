#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>




void Texture::load(std::string path, GLint interpolation, bool flip){
	this->interpolation = interpolation;
	int numComponents;
	stbi_set_flip_vertically_on_load(flip);
	const char* pathData = path.c_str();
	unsigned char* data = stbi_load(pathData, &width, &height, &numComponents, 0);
	if (data != nullptr)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		//Set alignment to be 1

		bind();

		//Set number of components by format of the texture
		GLuint format = GL_RGB;
		switch (numComponents)
		{
		case 4:
			format = GL_RGBA;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 2:
			format = GL_RG;
			break;
		case 1:
			format = GL_RED;
			break;
		default:
			std::cout << "Invalid Texture Format" << std::endl;
			break;
		};
		//Loads texture data into bound texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);

		// Clean up
		unbind();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	//Return to default alignment
		stbi_image_free(data);

	}
	else {
		throw std::runtime_error("Failed to read texture data from file!");
	}
}

void Texture::setUpInternal(int width, int height, GLint interpolation, GLuint internalFormat, GLuint format, GLenum type) {
	this->interpolation = interpolation;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		//Set alignment to be 1
	bind();

	//Loads texture data into bound texture
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);

	// Clean up
	unbind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	//Return to default alignment
	this->width = width;
	this->height = height;
}

void Texture::setBorderColour(glm::vec4 col) {
	bind();
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &col[0]);
	unbind();
}

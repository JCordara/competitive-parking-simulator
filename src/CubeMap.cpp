#include "CubeMap.h"
#include <stb/stb_image.h>
#include <iostream>


CubeMap::CubeMap(std::vector<std::string> path, GLint interpolation) {
	if (path.size() != 6) {
		std::cout << "Cube map must have 6 faces" << std::endl;
		return;
	}
	this->interpolation = interpolation;
	int numComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		//Set alignment to be 1
	bind();

	for (unsigned int i = 0; i < 6; i++){
		data = stbi_load(path[i].c_str(), &width, &height, &numComponents, 0);
		if (data != nullptr){
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
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
			);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, interpolation);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, interpolation);
		} else {
			throw std::runtime_error("Failed to read texture data from file!");
		}
	}
	// Clean up
	unbind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	//Return to default alignment
	stbi_image_free(data);
	paths = path;
}

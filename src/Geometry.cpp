#include "Geometry.h"

#include <utility>


GPU_Geometry::GPU_Geometry()
	: vao()
	, vertBuffer(0, 3, GL_FLOAT)
	, colourBuffer(1, 3, GL_FLOAT)
	, normalsBuffer(2, 3, GL_FLOAT)
	, texCoordBuffer(3, 2, GL_FLOAT)
	, indexBuffer(4, 2, GL_UNSIGNED_INT)
{}


void GPU_Geometry::setVerts(const std::vector<glm::vec3>& verts) {
	vertBuffer.uploadData(sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);
}

void GPU_Geometry::setCols(const std::vector<glm::vec3>& cols) {
	colourBuffer.uploadData(sizeof(glm::vec3) * cols.size(), cols.data(), GL_STATIC_DRAW);
}

void GPU_Geometry::setNormals(const std::vector<glm::vec3>& norms) {
	normalsBuffer.uploadData(sizeof(glm::vec3) * norms.size(), norms.data(), GL_STATIC_DRAW);
}

void GPU_Geometry::setTexCoords(const std::vector<glm::vec2>& texCoords) {
	texCoordBuffer.uploadData(sizeof(glm::vec2) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
}

void GPU_Geometry::setInds(const std::vector<GLuint>& inds) {
	indexBuffer.uploadData(sizeof(GLuint) * inds.size(), inds.data(), GL_STATIC_DRAW);
}

CPU_Geometry generatePlaneGeometry(glm::vec3 colour) {
	CPU_Geometry plane;
	plane.verts = {
		glm::vec3(0.5f,0.0f,0.5f),
		glm::vec3(0.5f,0.0f,-0.5f),
		glm::vec3(-0.5f,0.0f,-0.5f),
		glm::vec3(-0.5f,0.0f,0.5f),
	};
	plane.cols = {colour,colour,colour,colour};
	plane.normals = {glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f)};
	plane.ind = {
		0,1,2,
		0,2,3,
	};
	return plane;
}


CPU_Geometry generateCubeGeometry(glm::vec3 colour) {
	CPU_Geometry cube;
	cube.verts = {
		//   +X
		glm::vec3(0.5f,0.5f,-0.5f),
		glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(0.5f,-0.5f,0.5f),
		glm::vec3(0.5f,-0.5f,-0.5f),
		//   -X
		glm::vec3(-0.5f,0.5f,0.5f),
		glm::vec3(-0.5f,0.5f,-0.5f),
		glm::vec3(-0.5f,-0.5f,-0.5f),
		glm::vec3(-0.5f,-0.5f,0.5f),
		//   +Y
		glm::vec3(-0.5f,0.5f,-0.5f),
		glm::vec3(-0.5f,0.5f,0.5f),
		glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(0.5f,0.5f,-0.5f),
		//   -Y
		glm::vec3(0.5f,-0.5f,-0.5f),
		glm::vec3(0.5f,-0.5f,0.5f),
		glm::vec3(-0.5f,-0.5f,0.5f),
		glm::vec3(-0.5f,-0.5f,-0.5f),
		//   +Z
		glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(-0.5f,0.5f,0.5f),
		glm::vec3(-0.5f,-0.5f,0.5f),
		glm::vec3(0.5f,-0.5f,0.5f),
		//   -Z
		glm::vec3(-0.5f,0.5f,-0.5f),
		glm::vec3(0.5f,0.5f,-0.5f),
		glm::vec3(0.5f,-0.5f,-0.5f),
		glm::vec3(-0.5f,-0.5f,-0.5f),
	};
	for (int i = 0; i < 4 * 6; i++) cube.cols.push_back(colour);
	cube.normals = {
		glm::vec3(1.0f,0.0f,0.0f), glm::vec3(1.0f,0.0f,0.0f), glm::vec3(1.0f,0.0f,0.0f), glm::vec3(1.0f,0.0f,0.0f),
		glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(-1.0f,0.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f),
		glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,0.0f,1.0f),
		glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,0.0f,-1.0f)
	};
	for (int i = 0; i < 6; i++) {
		cube.ind.push_back(4 * i + 0);
		cube.ind.push_back(4 * i + 1);
		cube.ind.push_back(4 * i + 2);
		cube.ind.push_back(4 * i + 0);
		cube.ind.push_back(4 * i + 2);
		cube.ind.push_back(4 * i + 3);
	}
	return cube;
}



int FaceIndex(int offset, int i, int j) {
	return (offset + 1) * i + j;
}

CPU_Geometry generateSphereGeometry(glm::vec3 colour, unsigned int xFactor, unsigned int yFactor) {
	CPU_Geometry ret;
	float dP = M_PI / yFactor;
	float d0 = M_PI / xFactor;
	float dY = 1.0f / yFactor;
	float dX = 0.5f / xFactor;;
	glm::vec3 T3;
	for (int i = 0; i <= 2 * xFactor; i++) {
		for (int j = 0; j <= yFactor; j++) {
			if (j == yFactor) T3 = glm::vec3(0.0f, -1.0f, 0.0f);
			else T3 = glm::vec3(sinf(dP * j) * cosf(d0 * i), cosf(dP * j), sinf(dP * j) * sinf(d0 * i));
			ret.verts.push_back(T3);
			ret.normals.push_back(T3);
			ret.texCoords.push_back(glm::vec2(dX * (2 * xFactor - i), dY * (yFactor - j)));
			ret.cols.push_back(colour);
			if (i > 0 && j > 0) {
				ret.ind.push_back(FaceIndex(yFactor, i - 1, j - 1));
				ret.ind.push_back(FaceIndex(yFactor, i, j));
				ret.ind.push_back(FaceIndex(yFactor, i - 1, j));
				ret.ind.push_back(FaceIndex(yFactor, i - 1, j - 1));
				ret.ind.push_back(FaceIndex(yFactor, i, j - 1));
				ret.ind.push_back(FaceIndex(yFactor, i, j));
			}
		}
	}
	return ret;
}

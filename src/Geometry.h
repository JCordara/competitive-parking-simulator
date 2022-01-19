#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

// List of vertices and texture coordinates using std::vector and glm::vec3
struct CPU_Geometry {
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> cols;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<GLuint> ind;
};

// VAO and two VBOs for storing vertices and texture coordinates, respectively
class GPU_Geometry {

public:
	GPU_Geometry();

	// Public interface
	void bind() { vao.bind(); }

	void setVerts(const std::vector<glm::vec3>& verts);
	void setCols(const std::vector<glm::vec3>& cols);
	void setNormals(const std::vector<glm::vec3>& norms);
	void setTexCoords(const std::vector<glm::vec2>& texCoords);
	void setInds(const std::vector<GLuint>& inds);

private:
	// note: due to how OpenGL works, vao needs to be
	// defined and initialized before the vertex buffers
	VertexArray vao;

	VertexBuffer vertBuffer;
	VertexBuffer colourBuffer;
	VertexBuffer normalsBuffer;
	VertexBuffer texCoordBuffer;
	IndexBuffer indexBuffer;
};
CPU_Geometry generatePlaneGeometry(glm::vec3 colour);
CPU_Geometry generateCubeGeometry(glm::vec3 colour);
CPU_Geometry generateSphereGeometry(glm::vec3 colour, unsigned int xFactor, unsigned int yFactor);


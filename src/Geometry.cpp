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

CPU_Geometry generateTestCubeModel(glm::vec3 colour) {
	CPU_Geometry cube;
	cube.verts =	{
						glm::vec3(1.0f,1.0f,1.0f),
						glm::vec3(-1.0f,1.0f,1.0f),
						glm::vec3(-1.0f,-1.0f,1.0f),
						glm::vec3(1.0f,-1.0f,1.0f),
						glm::vec3(1.0f,1.0f,-1.0f),
						glm::vec3(-1.0f,1.0f,-1.0f),
						glm::vec3(-1.0f,-1.0f,-1.0f),
						glm::vec3(1.0f,-1.0f,-1.0f)
					};
	cube.cols =		{
						colour,
						colour,
						colour,
						colour,
						colour,
						colour,
						colour,
						colour
					};
	cube.normals =	{
						glm::normalize(glm::vec3(1.0f,1.0f,1.0f)),
						glm::normalize(glm::vec3(-1.0f,1.0f,1.0f)),
						glm::normalize(glm::vec3(-1.0f,-1.0f,1.0f)),
						glm::normalize(glm::vec3(1.0f,-1.0f,1.0f)),
						glm::normalize(glm::vec3(1.0f,1.0f,-1.0f)),
						glm::normalize(glm::vec3(-1.0f,1.0f,-1.0f)),
						glm::normalize(glm::vec3(-1.0f,-1.0f,-1.0f)),
						glm::normalize(glm::vec3(1.0f,-1.0f,-1.0f))
					};
	cube.ind =		{
						0,1,2,
						0,2,3,
						4,0,3,
						4,3,7,
						5,4,7,
						5,7,6,
						1,5,6,
						1,6,2,
						4,5,1,
						4,1,0,
						3,2,6,
						3,6,7
					};
	return cube;
}

CPU_Geometry generateTestPlaneModel(glm::vec3 colour) {
	CPU_Geometry plane;
	plane.verts =	{
						glm::vec3(1.0f,0.0f,1.0f),
						glm::vec3(-1.0f,0.0f,1.0f),
						glm::vec3(-1.0f,0.0f,-1.0f),
						glm::vec3(1.0f,0.0f,-1.0f),
					};
	plane.cols =		{
						colour,
						colour,
						colour,
						colour
					};
	plane.normals = {
						glm::normalize(glm::vec3(0.0f,1.0f,0.0f)),
						glm::normalize(glm::vec3(0.0f,1.0f,0.0f)),
						glm::normalize(glm::vec3(0.0f,1.0f,0.0f)),
						glm::normalize(glm::vec3(0.0f,1.0f,0.0f)),
					};
	plane.ind =		{
						0,1,2,
						0,2,3,
					};
	return plane;
}

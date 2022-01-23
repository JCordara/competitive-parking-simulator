#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Geometry.h"


class Model {
public:
	Model(CPU_Geometry cpuGeometry, glm::vec4 phongConstants, bool coloured);

	void setCPU_Geometry(CPU_Geometry cpuG) { cpuGeometry = cpuG; }
	void setPhongConstants(glm::vec4 pc) { phongConstants = pc; }
	void setColoured(bool c) { coloured = c; }

	CPU_Geometry getCPU_Geometry() { return cpuGeometry; }
	glm::vec4 getPhongConstants() { return phongConstants; }
	bool isColoured() { return coloured; }

	void prepareModelForRendering(GLint pongConstantsLocation, GLint colouredLocation, GPU_Geometry& drawGeometry);
	void renderModel(int instances);

private:
	CPU_Geometry cpuGeometry;
	glm::vec4 phongConstants;
	bool coloured;
};



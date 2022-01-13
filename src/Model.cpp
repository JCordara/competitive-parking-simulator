#pragma once
#include "Model.h"

Model::Model(CPU_Geometry cpuGeometry, glm::vec4 phongConstants, bool coloured) : cpuGeometry(cpuGeometry), phongConstants(phongConstants), coloured(coloured) {}


void Model::prepareModelForRendering(GLint phongConstantsLocation, GLint colouredLocation, GPU_Geometry& drawGeometry) {
	drawGeometry.setVerts(cpuGeometry.verts);
	drawGeometry.setCols(cpuGeometry.cols);
	drawGeometry.setNormals(cpuGeometry.normals);
	drawGeometry.setTexCoords(cpuGeometry.texCoords);
	drawGeometry.setInds(cpuGeometry.ind);
	drawGeometry.bind();
	glUniform1i(colouredLocation, (coloured) ? 1 : 0);
	glUniform4fv(phongConstantsLocation, 1, &phongConstants[0]);

}

void Model::renderModel(int instances) {
	glDrawElementsInstanced(GL_TRIANGLES, cpuGeometry.ind.size(), GL_UNSIGNED_INT, (void*)0, instances);
}

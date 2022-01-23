#pragma once
#include "Lighting.h"

PointLight::PointLight(glm::vec3 pos1, glm::vec3 col1, glm::vec3 attenuationConsts1) : pos(pos1), col(col1), attenuationConsts(attenuationConsts1) {
	calculateRadius();
}

void preparePointLightsForRendering(GLint lightPositionsLocation, GLint lightColoursLocation, GLint lightAttenuationConstaintsLocation, GLint radiusLocation, GLint numberOfLightsLocation, std::vector<PointLight>& pointLights) {
	std::vector<std::vector<GLfloat>> ret = std::vector<std::vector<GLfloat>>(4);
	ret[0] = std::vector<GLfloat>(3 * pointLights.size());
	ret[1] = std::vector<GLfloat>(3 * pointLights.size());
	ret[2] = std::vector<GLfloat>(3 * pointLights.size());
	ret[3] = std::vector<GLfloat>(pointLights.size());
	glm::vec3 temp;
	for (int i = 0; i < pointLights.size(); i++) {
		temp = pointLights[i].getPos();
		ret[0][3 * i + 0] = temp[0];
		ret[0][3 * i + 1] = temp[1];
		ret[0][3 * i + 2] = temp[2];
		temp = pointLights[i].getCol();
		ret[1][3 * i + 0] = temp[0];
		ret[1][3 * i + 1] = temp[1];
		ret[1][3 * i + 2] = temp[2];
		temp = pointLights[i].getAttenuationConsts();
		ret[2][3 * i + 0] = temp[0];
		ret[2][3 * i + 1] = temp[1];
		ret[2][3 * i + 2] = temp[2];
		ret[3][i] = pointLights[i].geRadius();
	}
	glUniform3fv(lightPositionsLocation, pointLights.size(), ret[0].data());
	glUniform3fv(lightColoursLocation, pointLights.size(), ret[1].data());
	glUniform3fv(lightAttenuationConstaintsLocation, pointLights.size(), ret[2].data());
	glUniform1fv(radiusLocation, pointLights.size(), ret[3].data());
	glUniform1i(numberOfLightsLocation, pointLights.size());
}

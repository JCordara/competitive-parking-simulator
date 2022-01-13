#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class PointLight {
public:

	PointLight(glm::vec3 pos1, glm::vec3 col1, glm::vec3 attenuationConsts1);

	void setPos(glm::vec3 pos1) { pos = pos1; }
	void setCol(glm::vec3 col1) { col = col1; }
	void setAttenuationConsts(glm::vec3 attenuationConsts1) { attenuationConsts = attenuationConsts1; }

	glm::vec3 getPos() { return pos; }
	glm::vec3 getCol() { return col; }
	glm::vec3 getAttenuationConsts() { return attenuationConsts; }

	void displacePos(glm::vec3 dpos) { pos += dpos; }

private:
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec3 attenuationConsts;
};

void preparePointLightsForRendering(GLint lightPositionsLocation, GLint lightColoursLocation, GLint lightAttenuationConstaintsLocation, GLint numberOfLightsLocation, std::vector<PointLight>& pointLights);

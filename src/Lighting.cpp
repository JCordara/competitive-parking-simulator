#pragma once
#include "Lighting.h"

PointLight::PointLight(glm::vec3 pos1, glm::vec3 col1, glm::vec3 attenuationConsts1) : pos(pos1), col(col1), attenuationConsts(attenuationConsts1) {
	calculateRadius();
}

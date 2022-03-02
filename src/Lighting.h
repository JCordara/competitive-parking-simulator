#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class PointLight {
public:

	PointLight( glm::vec3 col1, glm::vec3 attenuationConsts1);

	void setCol(glm::vec3 col1) { col = col1; }
	void setAttenuationConsts(glm::vec3 attenuationConsts1) {
		attenuationConsts = attenuationConsts1;
		calculateRadius();
	}

	glm::vec3 getCol() { return col; }
	glm::vec3 getAttenuationConsts() { return attenuationConsts; }
	float geRadius() { return radius; }

private:

	void calculateRadius(){
		float maxLight = std::fmaxf(std::fmaxf(col[0], col[1]), col[2]);
		if (attenuationConsts[2] < 0.00001f) {
			if (attenuationConsts[1] < 0.00001f) radius = std::numeric_limits<float>::max();
			else radius = (128.f * maxLight - attenuationConsts[0]) / attenuationConsts[1];
		}else {
			float alpha = 0.5f * attenuationConsts[1] / attenuationConsts[2];
			float beta = (attenuationConsts[0] - 128.f * maxLight) / attenuationConsts[2];
			float det = alpha * alpha - beta;
			if (det < 0.f) radius = 0.0f;
			else radius = std::sqrtf(det) - alpha;
		}
	}

	glm::vec3 col;
	glm::vec3 attenuationConsts;
	float radius;
};

class DirectionalLight {
public:
	DirectionalLight(glm::vec3 col) : col(col) {}
	void setCol(glm::vec3 col) { this->col = col; }
	glm::vec3 getCol() { return col; }
private:
	glm::vec3 col;
};

class SpotLight : public PointLight {
public:
	SpotLight(glm::vec3 col, glm::vec3 attenuationConsts, float innerAngle, float outerAngle) :
		PointLight(col, attenuationConsts), innerAngle(innerAngle), outerAngle(outerAngle){
		cosInnerAngle = cosf(innerAngle);
		cosOuterAngle = cosf(outerAngle);
	}
	void setInnerAngle(float angle) { this->innerAngle = angle; cosInnerAngle = cosf(angle); }
	void setOuterAngle(float angle) { this->outerAngle = angle; cosOuterAngle = cosf(angle); }
	float getInnerAngle() { return innerAngle; }
	float getCosInnerAngle() { return cosInnerAngle; }
	float getOuterAngle() { return outerAngle; }
	float getCosOuterAngle() { return cosOuterAngle; }
private:
	float innerAngle, cosInnerAngle, outerAngle, cosOuterAngle;
};

class AmbientLight {
public:
	AmbientLight(glm::vec3 col) : col(col) {}
	void setCol(glm::vec3 col) { this->col = col; }
	glm::vec3 getCol() { return col; }
private:
	glm::vec3 col;
};



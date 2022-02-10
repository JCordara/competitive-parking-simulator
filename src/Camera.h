#pragma once
#include <GL/glew.h>

#include "stdafx.h"

class Camera {
public:

	Camera(glm::vec3 pos, float theta, float phi, float fov_width, float aspectRatio_height, float nearPlane, float farPlane, bool orthograpic); //For perspective xor orthographic
	Camera(glm::vec3 pos, float theta, float phi, float fov, float aspectRatio, float width, float height, float nearPlane, float farPlane); //For both

	glm::mat4 getView();
	glm::mat4 getPerspectiveProjection();
	glm::mat4 getOrthographicProjection();

	void setPos(glm::vec3 pos) { this->pos = pos; }
	void setTheta(float theta) { this->theta = theta; }
	void setPhi(float phi) { this->phi = phi; }

	void setFov(float fov) { this->fov = fov; }
	void setAspectRatio(float aspectRatio) {
		if (!screenControlled) {
			this->aspectRatio = aspectRatio;
		}
	}
	void setWidth(float width) { this->width = width; }
	void setHeight(float height) { this->height = height; }
	void setNearPlane(float nearPlane) { this->nearPlane = nearPlane; }
	void setFarPlane(float farPlane) { this->farPlane = farPlane; }

	glm::vec3 getPos() { return pos; }
	float getTheta() { return theta; }
	float getPhi() { return phi; }

	float getFov() { this->fov = fov; }
	float getAspectRatio() { return aspectRatio; }
	float getWidth() { return width; }
	float getHeight() { return height; }
	float getNearPlane() { return nearPlane; }
	float getFarPlane() { return farPlane; }

	void displacePos(glm::vec3 dpos) { pos += dpos; }
	void incrementTheta(float dt) { theta += dt; }
	void incrementPhi(float dp) { phi += dp; }
	glm::vec3 getHeadingVector() { return glm::vec3(-sinf(theta) * cosf(phi), sinf(phi), -cosf(theta) * cosf(phi)); }

	void usePerspective(GLint viewMatrixLocation, GLint projectionMatrixLocation, GLint cameraPostionLocation);
	void useOrthographic(GLint viewMatrixLocation, GLint projectionMatrixLocation);
	void windowSizeChanged(int width, int height);
private:
	glm::vec3 pos;
	bool screenControlled;
	float theta;
	float phi;

	float fov;
	float aspectRatio;

	float width;
	float height;

	float nearPlane;
	float farPlane;
};

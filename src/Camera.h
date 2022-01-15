#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera {
public:

	Camera(glm::vec3 pos, float theta, float phi, float fov, float aspectRatio, float nearPlane, float farPlane);

	glm::mat4 getView();

	void setPos(glm::vec3 pos) { this->pos = pos; }
	void setTheta(float theta) { this->theta = theta; }
	void setPhi(float phi) { this->phi = phi; }
	void setFov(float fov) { this->fov = fov; }
	void setAspectRatio(float aspectRatio) {
		if (!screenControlled) {
			this->aspectRatio = aspectRatio;
		}
	}
	void setNearPlane(float nearPlane) { this->nearPlane = nearPlane; }
	void setFarPlane(float farPlane) { this->farPlane = farPlane; }

	glm::vec3 getPos() { return pos; }
	float getTheta() { return theta; }
	float getPhi() { return phi; }
	float getFov() { this->fov = fov; }
	float getAspectRatio() { return aspectRatio; }
	float getNearPlane() { return nearPlane; }
	float getFarPlane() { return farPlane; }

	void displacePos(glm::vec3 dpos) { pos += dpos; }
	void incrementTheta(float dt) { theta += dt; }
	void incrementPhi(float dp) { phi += dp; }

	void usePerspective(GLint viewMatrixLocation, GLint projectionMatrixLocation, GLint cameraPostionLocation);
	//TO-DO: void useOrthographic(GLint viewMatrixLocation, GLint projectionMatrixLocation, float width, float height, float nearPlane, float farPlane);
	void windowSizeChanged(int width, int height);
private:
	glm::vec3 pos;
	bool screenControlled;
	float theta;
	float phi;
	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
};

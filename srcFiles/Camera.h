#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera {
public:

	Camera(glm::vec3 pos1, float t1, float p1);

	glm::mat4 getView();

	void setPos(glm::vec3 pos1) { pos = pos1; }
	void setTheta(float t1) { theta = t1; }
	void setPhi(float p1) { phi = p1; }

	glm::vec3 getPos() { return pos; }
	float getTheta() { return theta; }
	float getPhi() { return phi; }

	void displacePos(glm::vec3 dpos) { pos += dpos; }
	void incrementTheta(float dt) { theta += dt; }
	void incrementPhi(float dp) { phi += dp; }

	void usePerspective(GLint viewMatrixLocation, GLint projectionMatrixLocation, GLint cameraPostionLocation, float fov, float aspectRatio, float nearPlane, float farPlane);
	//TO-DO: void useOrthographic(GLint viewMatrixLocation, GLint projectionMatrixLocation, float width, float height, float nearPlane, float farPlane);

private:
	glm::vec3 pos;
	float theta;
	float phi;

};

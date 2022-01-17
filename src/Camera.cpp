#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 pos, float theta, float phi, float fov, float aspectRatio, float nearPlane, float farPlane):
	pos(pos), theta(theta), phi(phi), fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane){
	screenControlled = false;
}

//Default is looking directly into the -z direction
//https://www.evl.uic.edu/ralph/508S98/coordinates.html#:~:text=The%20default%20coordinate%20system%20in%20OpenGL(TM)%20is%20right%2D,negative%20z%20axis%20points%20forward.
glm::mat4 Camera::getView() {
	float cp = cosf(phi);
	float sp = sinf(phi);
	float ct = cosf(theta);
	float st = sinf(theta);
	return glm::lookAt(pos, pos + glm::vec3(-st * cp, sp, -ct * cp), glm::vec3(st * sp, cp, ct * sp));
}

void Camera::usePerspective(GLint viewMatrixLocation, GLint projectionMatrixLocation, GLint cameraPostionLocation){
	glm::mat4 persepective = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &getView()[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &persepective[0][0]);
	glUniform3fv(cameraPostionLocation, 1, &getPos()[0]);
}

void Camera::windowSizeChanged(int width, int height) {
	if (height == 0) {
		aspectRatio = 0.0f;
		return;
	}
	aspectRatio = ((float)width)/((float)height);
}

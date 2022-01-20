#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 pos, float theta, float phi, float fov_width, float aspectRatio_height, float nearPlane, float farPlane, bool orthographic):
	pos(pos), theta(theta), phi(phi), nearPlane(nearPlane), farPlane(farPlane){
	if (orthographic) {
		width = fov_width;
		height = aspectRatio_height;
		fov = 0.0f;
		aspectRatio = 0.0f;
	}
	else {
		width = 0.0f;
		height = 0.0f;
		fov = fov_width;
		aspectRatio = aspectRatio_height;
	}
	screenControlled = false;
}

Camera::Camera(glm::vec3 pos, float theta, float phi, float fov, float aspectRatio, float width, float height, float nearPlane, float farPlane) :
	pos(pos), theta(theta), phi(phi), fov(fov), aspectRatio(aspectRatio), width(width), height(height), nearPlane(nearPlane), farPlane(farPlane) {
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

glm::mat4 Camera::getPerspectiveProjection() {
	return glm::perspective(fov, aspectRatio, nearPlane, farPlane);;
}

glm::mat4 Camera::getOrthographicProjection() {
	return glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearPlane, farPlane);
}

void Camera::usePerspective(GLint viewMatrixLocation, GLint projectionMatrixLocation, GLint cameraPostionLocation){
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &getView()[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &getPerspectiveProjection()[0][0]);
	glUniform3fv(cameraPostionLocation, 1, &getPos()[0]);
}

void Camera::useOrthographic(GLint viewMatrixLocation, GLint projectionMatrixLocation) {
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &getView()[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &getOrthographicProjection()[0][0]);
}

void Camera::windowSizeChanged(int width, int height) {
	if (height == 0) {
		aspectRatio = 0.0f;
		return;
	}
	aspectRatio = ((float)width)/((float)height);
}

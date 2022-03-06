#include "CameraComponent.h"

CameraComponent::CameraComponent(Entity & parent)
	: BaseComponent(parent)
{}

ComponentEnum CameraComponent::getType() {
	return ComponentEnum::camera;
}

CameraComponent::~CameraComponent() {
	// Nothing to do here yet
}

void CameraComponent::setOrthographicCamera(float width, float height, float nearPlane, float farPlane) {
	camera = make_shared<Camera>(glm::vec3(0.f), 0.f, 0.f, width, height, nearPlane, farPlane, true);
	purpose = CameraPurpose::shadowMap;
}
void CameraComponent::setPerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane) {
	camera = make_shared<Camera>(glm::vec3(0.f), 0.f, 0.f, fov, aspectRatio, nearPlane, farPlane, false);
	purpose = CameraPurpose::render;
}

void CameraComponent::updateOrthographicCamera(float width, float height, float nearPlane, float farPlane) {
	if (CameraPurpose::shadowMap == purpose) {
		camera->setWidth(width);
		camera->setHeight(height);
		camera->setNearPlane(nearPlane);
		camera->setFarPlane(farPlane);
	}
}
void CameraComponent::updatePerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane) {
	if (CameraPurpose::render == purpose) {
		camera->setFov(fov);
		camera->setAspectRatio(aspectRatio);
		camera->setNearPlane(nearPlane);
		camera->setFarPlane(farPlane);
	}
}

glm::mat4 CameraComponent::getViewMatrix(glm::mat4 cameraModelTransformation) {
	glm::vec3 forward = glm::normalize(glm::vec3(cameraModelTransformation * glm::vec4(0.f, 0.f, -1.f, 0.f)));
	glm::vec3 up = glm::normalize(glm::vec3(cameraModelTransformation * glm::vec4(0.f, 1.f, 0.f, 0.f)));
	glm::vec3 eye = glm::vec3(cameraModelTransformation * glm::vec4(0.f, 0.f, 0.f, 1.f));
	return glm::lookAt(eye, eye + forward, up);
}
glm::mat4 CameraComponent::getProjectionMatrix() {
	if (CameraPurpose::render == purpose)
		return camera->getPerspectiveProjection();
	if (CameraPurpose::shadowMap == purpose)
		return camera->getOrthographicProjection();
	return glm::mat4(1.f);
}
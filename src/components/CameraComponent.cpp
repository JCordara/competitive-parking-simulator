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

glm::mat4 CameraComponent::getViewMatrix() {
	return glm::mat4(1.f);//the transform element will do this
}
glm::mat4 CameraComponent::getProjectionMatrix() {
	if (CameraPurpose::render == purpose)
		return camera->getPerspectiveProjection();
	if (CameraPurpose::shadowMap == purpose)
		return camera->getOrthographicProjection();
	return glm::mat4(1.f);
}

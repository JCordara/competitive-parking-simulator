#include "CameraComponent.h"

CameraComponent::CameraComponent(shared_ptr<Entity> parent)
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
	isPerspective = false;
}
void CameraComponent::setPerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane) {
	camera = make_shared<Camera>(glm::vec3(0.f), 0.f, 0.f, fov, aspectRatio, nearPlane, farPlane, false);
	isPerspective = true;
}

void CameraComponent::updateOrthographicCamera(float width, float height, float nearPlane, float farPlane) {
	camera->setWidth(width);
	camera->setHeight(height);
	camera->setNearPlane(nearPlane);
	camera->setFarPlane(farPlane);
}
void CameraComponent::updatePerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane) {
	camera->setFov(fov);
	camera->setAspectRatio(aspectRatio);
	camera->setNearPlane(nearPlane);
	camera->setFarPlane(farPlane);
}

void CameraComponent::setPurpose(CameraPurpose p) {
	purpose = p;
}

glm::mat4 CameraComponent::getViewMatrix(glm::mat4 cameraModelTransformation) {
	glm::vec3 forward = glm::normalize(glm::vec3(cameraModelTransformation * glm::vec4(0.f, 0.f, -1.f, 0.f)));
	glm::vec3 up = glm::normalize(glm::vec3(cameraModelTransformation * glm::vec4(0.f, 1.f, 0.f, 0.f)));
	glm::vec3 eye = glm::vec3(cameraModelTransformation * glm::vec4(0.f, 0.f, 0.f, 1.f));
	return glm::lookAt(eye, eye + forward, up);
}
glm::mat4 CameraComponent::getProjectionMatrix() {
	if (purpose != CameraPurpose::unknown){
		if (isPerspective)
			return camera->getPerspectiveProjection();
		return camera->getOrthographicProjection();
	}
	return glm::mat4(1.f);
}

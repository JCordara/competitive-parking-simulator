#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "Components.h"
#include <Camera.h>

enum class CameraPurpose {
	render,
	shadowMap,
	menu,
	shadowMapMenu,
	unknown
};

class CameraComponent : public BaseComponent {
public:
	CameraComponent(weak_ptr<Entity> parent);

	void setOrthographicCamera(float width, float height, float nearPlane, float farPlane);
	void setPerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane);
	void updateOrthographicCamera(float width, float height, float nearPlane, float farPlane);
	void updatePerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane);
	void windowSizeChanged(int width, int height) { camera->windowSizeChanged(width, height); }
	float getNearClipPlane() { return camera->getNearPlane(); }
	float getFarClipPlane() { return camera->getFarPlane(); }
	void setFov(float fov) { camera->setFov(fov); }
	void setPurpose(CameraPurpose p);
	CameraPurpose getPurpose() { return purpose; }
	glm::mat4 getViewMatrix(glm::mat4 cameraModelTransformation);
	glm::mat4 getProjectionMatrix();

	~CameraComponent();
	static ComponentEnum getType();
	
private:
	std::shared_ptr<Camera> camera;
	CameraPurpose purpose = CameraPurpose::unknown;

	bool isPerspective = false;
};

#endif // CAMERA_COMPONENT_H

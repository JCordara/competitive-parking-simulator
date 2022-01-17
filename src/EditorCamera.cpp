#include "EditorCamera.h"

// Constructor initializes camera on z axis looking at target
// TODO: Get rid of magic numbers
EditorCamera::EditorCamera() {

	// Set default values for camera position and options
	zoomAmount = default_cam_init_zoom;
	pitch = default_cam_init_pitch;
	yaw = default_cam_init_yaw;
	mouseSensitivity = default_cam_init_sensitivity * 500.0f; // Magic number
	scrollSensitivity = 0.3f; // Magic number - makes for natural scrolling
	moveSpeed = default_cam_init_speed;

	// Default view matrix (identity)
	viewMatrix = glm::mat4(1.0f);

	// Initialize important reference vectors
	worldUp = up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::vec3(1.0f, 0.0f, 0.0f);
	target = glm::vec3(0.0f, 0.0f, 0.0f);

	// Calculate initial values
	updateRadius();
	updateVectors();
	updatePosition();
}

// Initialize camera with a point to look at
EditorCamera::EditorCamera(glm::vec3 initLookAt) : EditorCamera() {
	target = initLookAt;
	updateRadius();
	updateVectors();
	updatePosition();
}


// Utility functions -----------------------------------------------

void EditorCamera::rotateAroundTarget(double xOffset, double yOffset) {

	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw   += xOffset * Time::delta();
	pitch += yOffset * Time::delta();

	// Clamp pitch between looking straight up and straight down
	if (pitch >  89.99f) pitch =  89.99f;
	if (pitch < -89.99f) pitch = -89.99f;

	updatePosition();
	updateVectors();
}

// Move the camera without rotating
void EditorCamera::move(double xOffset, double yOffset) {

	// TODO: fix this - it's screen dependent (window size, resolution)
	double moveSensitivity = mouseSensitivity * 0.4f;

	// Move both the target (being looked at) and position (camera position)
	// by the same amount
	float xAmount = static_cast<float>(
		xOffset * moveSensitivity * moveSpeed * Time::delta());
	float yAmount = static_cast<float>(
		yOffset * moveSensitivity * moveSpeed * Time::delta());

	// Move lookat target by specified amount
	target   += right * xAmount;
	target   -= up    * yAmount;

	updatePosition();
}

void EditorCamera::zoom(double offset) {
	zoomAmount += offset * scrollSensitivity;

	// Clamp zoom min and max
	if (zoomAmount > 10.0f) zoomAmount = 10.0f;
	if (zoomAmount < 1.0f) zoomAmount = 1.0f;

	updateRadius();
	updatePosition();
}

// Move look target back to origin
void EditorCamera::resetTarget() {
	target.x = 0.0f;
	target.y = 0.0f;
	target.z = 0.0f;

	updateVectors();
	updatePosition();
}


// Accessor functions ---------------------------------------

glm::mat4 EditorCamera::getViewMatrix() {
	/* viewMatrix = glm::lookAt(position, position + front, up); */
	viewMatrix = lookAt(position, target, up);
	return viewMatrix;
}

glm::vec3 EditorCamera::getPosition() const {
	return position;
}

glm::vec3 EditorCamera::getViewDirection() const {
	return glm::normalize(position - target);
}

void EditorCamera::setLookAt(glm::vec3 newLookAt) {
	target = newLookAt;
	updateVectors();
	updatePosition();
}

void EditorCamera::setSensitivity(double newSensitivity) {
	mouseSensitivity = newSensitivity * 60.0f;
}


// Private functions --------------------------------

// Recalculate up and right vectors relative to camera
void EditorCamera::updateVectors() {
	glm::vec3 frontDirection = getViewDirection();
	right = glm::normalize(glm::cross(frontDirection, worldUp));
	up = glm::normalize(glm::cross(right, frontDirection));

}

// Given radius, pitch, yaw, and target, recalculate camera position
void EditorCamera::updatePosition() {
	position.x = -radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	position.y = -radius * sin(glm::radians(pitch));
	position.z = radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	position += target;
}

// Calculate distance of camera from target based on zoom amount
// TODO: replace magic numbers with meaningful values
void EditorCamera::updateRadius() {
	
	// zoomAmount is clamped to range [1.0, 10.0]
	// Coefficient is a value in range [81.0, 0.0] on a gradually flattened curve
	double coefficient = pow((zoomAmount - 10.0), 2);

	// Lowers max radius and sets minimum camera distance from target
	radius = 0.25 * coefficient + 1;

}

glm::mat4 EditorCamera::lookAt(glm::vec3 camPosition, glm::vec3 camTarget, glm::vec3 camUp) {
	glm::vec3 camDirection = glm::normalize(camPosition - camTarget);
	glm::vec3 camRight = glm::normalize(glm::cross(camUp, camDirection));
	
	glm::mat4 lookAtMatrix1 = glm::mat4(1.0f);
	lookAtMatrix1[0][0] = camRight.x;
	lookAtMatrix1[1][0] = camRight.y;
	lookAtMatrix1[2][0] = camRight.z;
	lookAtMatrix1[0][1] = camUp.x;
	lookAtMatrix1[1][1] = camUp.y;
	lookAtMatrix1[2][1] = camUp.z;
	lookAtMatrix1[0][2] = camDirection.x;
	lookAtMatrix1[1][2] = camDirection.y;
	lookAtMatrix1[2][2] = camDirection.z;

	glm::mat4 lookAtMatrix2 = glm::mat4(1.0f);
	lookAtMatrix2[3][0] = camPosition.x * -1;
	lookAtMatrix2[3][1] = camPosition.y * -1;
	lookAtMatrix2[3][2] = camPosition.z * -1;

	return lookAtMatrix1 * lookAtMatrix2;
}

void EditorCamera::setZoom(double v) {
	zoomAmount = v;
	updateRadius();
	updatePosition();
}

void EditorCamera::setPitch(double v) {
	pitch = v;
	updateVectors();
	updatePosition();
}

void EditorCamera::setYaw(double v) {
	yaw = v;
	updateVectors();
	updatePosition();
}

#include "EditorCamera.h"

// Constructor initializes camera on z axis looking at target
// TODO: Get rid of magic numbers
EditorCamera::EditorCamera()

	: zoomAmount(default_cam_init_zoom)
	, pitch(default_cam_init_pitch)
	, yaw(default_cam_init_yaw)
	, mouseSensitivity(default_cam_init_sensitivity)
	, scrollSensitivity(0.3f) // Magic number - makes for natural scrolling
	, moveSpeed(default_cam_init_speed)

	, viewMatrix(glm::mat4(1.0f)) // Default view matrix (identity)

	// Initialize important reference vectors
	, worldUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, up(worldUp)
	, right(glm::vec3(1.0f, 0.0f, 0.0f))
	, target(glm::vec3(0.0f, 0.0f, 0.0f))

	// Embedded controller variables
	, c_MB1Down_(false)
	, c_MB2Down_(false)
{
	// Calculate initial values
	updateRadius();
	updatePosition();
	updateVectors();
}

// Initialize camera with a point to look at
EditorCamera::EditorCamera(glm::vec3 initLookAt) : EditorCamera() {
	target = initLookAt;
	updateRadius();
	updatePosition();
	updateVectors();
}


// Utility functions -----------------------------------------------

void EditorCamera::rotateAroundTarget(double xOffset, double yOffset) {

	// Only when right mouse button is down
	if (c_MB2Down_)
	{
		// TODO: fix this - it's screen dependent (window size, resolution)
		double sensitivityFactor = mouseSensitivity * 2.0;

		double deltaX = xOffset - c_mouseLastX_;
		double deltaY = yOffset - c_mouseLastY_;
		
		deltaX *= sensitivityFactor;
		deltaY *= sensitivityFactor;

		yaw   += deltaX * Time::delta();
		pitch -= deltaY * Time::delta();

		// Clamp pitch between looking straight up and straight down
		if (pitch >  89.99f) pitch =  89.99f;
		if (pitch < -89.99f) pitch = -89.99f;

		updatePosition();
		updateVectors();
	}

	// Update last frame's mouse positions
	c_mouseLastX_ = xOffset;
	c_mouseLastY_ = yOffset;
}

// Move the camera without rotating
void EditorCamera::move(double xOffset, double yOffset) {

	// Only when left mouse button is down
	if (!c_MB1Down_) return;

	double deltaX = xOffset - c_mouseLastX_;
	double deltaY = yOffset - c_mouseLastY_;

	// Move both the target (being looked at) and position (camera position)
	// by the same amount
	float xAmount = static_cast<float>(
		deltaX * mouseSensitivity * moveSpeed * Time::delta());
	float yAmount = static_cast<float>(
		deltaY * mouseSensitivity * moveSpeed * Time::delta());

	// Move lookat target by specified amount
	target   += right * xAmount;
	target   += up    * yAmount;

	updatePosition();

}

void EditorCamera::zoom(double, double yOffset) {
	zoomAmount += yOffset * scrollSensitivity;

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

	updateRadius();
	updatePosition();
	updateVectors();
}


// Accessor functions ---------------------------------------

glm::mat4 EditorCamera::getViewMatrix() {
	//viewMatrix = glm::lookAt(position, target, up);
	viewMatrix = lookAt(position, target, up);
	return viewMatrix;
}

glm::vec3 EditorCamera::getPosition() const {
	return position;
}

glm::vec3 EditorCamera::getViewDirection() const {
	return glm::normalize(position - target);
}

glm::vec3 EditorCamera::getUpDirection() const {
	return glm::normalize(up);
}

void EditorCamera::setLookAt(glm::vec3 newLookAt) {
	target = newLookAt;
	updateRadius();
	updatePosition();
	updateVectors();
}

void EditorCamera::setSensitivity(double newSensitivity) {
	mouseSensitivity = newSensitivity;
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
	// Factor out common values
	double d_pitch = glm::radians(pitch);
	double d_yaw = glm::radians(yaw);
	double cosPitch = cos(d_pitch);

	// Calculate relative position
	position.x = static_cast<float>(-radius * sin(d_yaw) * cosPitch);
	position.y = static_cast<float>(-radius * sin(d_pitch));
	position.z = static_cast<float>( radius * cos(d_yaw) * cosPitch);

	// Calculate world position
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
	updateRadius();
	updatePosition();
	updateVectors();
}

void EditorCamera::setYaw(double v) {
	yaw = v;
	updateRadius();
	updatePosition();
	updateVectors();
}


/* Embedded controller functions */

void EditorCamera::button1Down() {
	c_MB1Down_ = true;
}
void EditorCamera::button1Up() {
	c_MB1Down_ = false;
}
void EditorCamera::button2Down() {
	c_MB2Down_ = true;
}
void EditorCamera::button2Up() {
	c_MB2Down_ = false;
}
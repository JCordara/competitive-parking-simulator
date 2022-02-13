#include "EditorCamera.h"

// Constructor initializes camera on z axis looking at target
// TODO: Get rid of magic numbers
EditorCamera::EditorCamera()

	: zoomAmount_(default_cam_init_zoom)
	, zoomScale_(default_cam_init_zoom_scale)
	, pitch_(default_cam_init_pitch)
	, yaw_(default_cam_init_yaw)
	, mouseSensitivity_(default_cam_init_sensitivity)
	, scrollSensitivity_(0.3f) // Magic number - makes for natural scrolling
	, moveSpeed_(default_cam_init_speed)

	// Default view matrix (identity)
	, viewMatrix_(glm::mat4(1.0f))

	// Initialize important reference vectors
	, worldUp_(glm::vec3(0.0f, 1.0f, 0.0f))
	, up_(worldUp_)
	, right_(glm::vec3(1.0f, 0.0f, 0.0f))
	, target_(glm::vec3(0.0f, 0.0f, 0.0f))

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
	target_ = initLookAt;
	updateRadius();
	updatePosition();
	updateVectors();
}


// Utility functions -----------------------------------------------

void EditorCamera::rotateAroundTarget(double xOffset, double yOffset) {

	double sensitivityFactor = mouseSensitivity_ * 0.25;
	
	xOffset *= sensitivityFactor;
	yOffset *= sensitivityFactor;

	yaw_   += xOffset;
	pitch_ -= yOffset;

	// Clamp pitch between looking straight up and straight down
	if (pitch_ >  89.99f) pitch_ =  89.99f;
	if (pitch_ < -89.99f) pitch_ = -89.99f;

	updatePosition();
	updateVectors();
}

// Move the camera without rotating
void EditorCamera::move(double xOffset, double yOffset) {

	// Move both the target (being looked at) and position (camera position)
	// by the same amount
	float xAmount = static_cast<float>(
		xOffset * mouseSensitivity_ * moveSpeed_);
	float yAmount = static_cast<float>(
		yOffset * mouseSensitivity_ * moveSpeed_);

	// Move lookat target by specified amount
	target_   += right_ * xAmount;
	target_   += up_    * yAmount;

	updatePosition();

}

void EditorCamera::zoom(double, double yOffset) {
	zoomAmount_ += yOffset * scrollSensitivity_;

	// Clamp zoom min and max
	if (zoomAmount_ > default_cam_init_max_zoom) 
		zoomAmount_ = default_cam_init_max_zoom;
	if (zoomAmount_ < 0.0f) 
		zoomAmount_ = 0.0f;

	updateRadius();
	updatePosition();
}

// Move look target back to origin
void EditorCamera::resetTarget() {
	target_.x = 0.0f;
	target_.y = 0.0f;
	target_.z = 0.0f;

	updateRadius();
	updatePosition();
	updateVectors();
}


// Accessor functions ---------------------------------------

glm::mat4 EditorCamera::getViewMatrix() {
	//viewMatrix = glm::lookAt(position, target, up);
	viewMatrix_ = lookAt(position_, target_, up_);
	return viewMatrix_;
}

glm::vec3 EditorCamera::getPosition() const {
	return position_;
}

glm::vec3 EditorCamera::getViewDirection() const {
	return glm::normalize(position_ - target_);
}

glm::vec3 EditorCamera::getUpDirection() const {
	return glm::normalize(up_);
}

void EditorCamera::setLookAt(glm::vec3 newLookAt) {
	target_ = newLookAt;
	updateRadius();
	updatePosition();
	updateVectors();
}

void EditorCamera::setZoomScale(double s) {
	// Add a little bit so max zoom doesnt clip
	zoomScale_ = s + 0.001;
	updateVectors();
	updatePosition();
	updateRadius();
}

void EditorCamera::setSensitivity(double newSensitivity) {
	mouseSensitivity_ = newSensitivity * 60.0;
}


// Private functions --------------------------------

// Recalculate up and right vectors relative to camera
void EditorCamera::updateVectors() {
	glm::vec3 frontDirection = getViewDirection();

	right_ = glm::normalize(glm::cross(frontDirection, worldUp_));
	up_    = glm::normalize(glm::cross(right_, frontDirection));

}

// Given radius, pitch, yaw, and target, recalculate camera position
void EditorCamera::updatePosition() {
	// Factor out common values
	double d_pitch = glm::radians(pitch_);
	double d_yaw = glm::radians(yaw_);
	double cosPitch = cos(d_pitch);

	// Calculate relative position
	position_.x = static_cast<float>(-radius_ * sin(d_yaw) * cosPitch);
	position_.y = static_cast<float>(-radius_ * sin(d_pitch));
	position_.z = static_cast<float>( radius_ * cos(d_yaw) * cosPitch);

	// Calculate world position
	position_ += target_;
}

// Update distance of camera from target based on zoom amount and zoom scale
void EditorCamera::updateRadius() {
	// Note: zoomAmount_ ranges from 0 to default_cam_init_max_zoom
	double coef = default_cam_init_max_zoom - zoomAmount_; // 0 = zoomed in

	radius_ = pow((coef), 3); // Zoom increments are larger on the large scale
	radius_ *= 0.025; // Scale down to good size
	radius_ += zoomScale_; // Minimum distance from zoom target
}

glm::mat4 EditorCamera::lookAt(
	glm::vec3 camPosition, 
	glm::vec3 camTarget, 
	glm::vec3 camUp) 
{
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
	zoomAmount_ = v;
	updateRadius();
	updatePosition();
}

void EditorCamera::setPitch(double v) {
	pitch_ = v;
	updateRadius();
	updatePosition();
	updateVectors();
}

void EditorCamera::setYaw(double v) {
	yaw_ = v;
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



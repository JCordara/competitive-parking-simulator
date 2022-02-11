// editor_camera.h / editor_camera.cpp are files I wrote a while back
// implementing a version of the "trackball" style camera. It has been
// modified to be easily integrated into assignment 3

#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "Common.h"
#include "Framework.h"


/* --- Default intial values --- */

// Initial yaw (y-axis rotation)
const double default_cam_init_yaw         = 0.0;

// Initial pitch (x-axis rotation)
const double default_cam_init_pitch       = 0.0;

// Camera move speed multiplier
const double default_cam_init_speed       = 0.5;

// Mouse sensitivity multiplier
const double default_cam_init_sensitivity = 1.0;

// Initial zoom scale
const double default_cam_init_zoom_scale = 2.0;

// Initial value for max_zoom
const double default_cam_init_max_zoom = 15.0;

// Initial zoom amount (clamped from 0 - max_zoom)
const double default_cam_init_zoom        = 1.0;


class EditorCamera {

public:

	// --- Constructors ---
	// Initialize camera at position (0, 0, z) facing origin
	EditorCamera();

	// Initialize camera at position (0, 0, z) relative to/facing initLookAt
	EditorCamera(glm::vec3 initLookAt);

	
	// --- Utility functions ---
	// Rotate camera around target 
	// given mouse offset from last frame in x and y coordinates
	void rotateAroundTarget(double xOffset, double yOffset);

	// Move the camera along the x-y plane of the view frame
	// given mouse offset from last frame in x and y coordinates
	void move(double xOffset, double yOffset);

	// Update the distance from the target given scroll wheel offset
	void zoom(double, double yOffset);

	// Reset lookat target to origin (0,0,0)
	void resetTarget();


	// --- Accessor functions ---
	// Calculate and return the view matrix
	glm::mat4 getViewMatrix();

	// Return current position of camera
	glm::vec3 getPosition() const;

	// Return current view direction vector of camera (normalized)
	glm::vec3 getViewDirection() const;

	// Return current up direction vector of camera (normalized)
	glm::vec3 getUpDirection() const;

	// Direct access to underlying camera settings
	void setLookAt(glm::vec3 newLookAt);
	void setSensitivity(double);
	void setZoom(double v);
	void setZoomScale(double s);
	void setPitch(double v);
	void setYaw(double v);

	// Simple debug method to print out all camera-relative values
	void printDebugInfo() {
		printf("\nradius: %f\n", radius_);
		printf("pitch:  %f\n", pitch_);
		printf("yaw:    %f\n", yaw_);
		printf("position: (%.2f, %.2f, %.2f)\n", position_.x, position_.y, position_.z);
		printf("target:   (%.2f, %.2f, %.2f)\n", target_.x, target_.y, target_.z);
	};

	/* Embedded controller functions */
	void button1Down();
	void button1Up();
	void button2Down();
	void button2Up();

private:

	/* Embedded controller variables */
	bool c_MB1Down_;		// Is left mouse button down
	bool c_MB2Down_;		// Is right mouse button down
	double c_mouseLastX_;	// Mouse X coordinate from last frame
	double c_mouseLastY_;	// Mouse Y coordinate from last frame

	double radius_; 		// Distance of camera from target
	double zoomAmount_; 	// Current zoom of the camera
	double zoomScale_; 	// Current zoom of the camera
	double pitch_; 		// Up/down angle of camera
	double yaw_;			// Left/right angle of camera
	double moveSpeed_; 	// Sensitivity of camera translation movements

	// Scaling factors for mouse sensitivity
	double mouseSensitivity_;
	double scrollSensitivity_;

	glm::vec3 position_; // Current position of camera
	glm::vec3 worldUp_;	// Up direction vector relative to world (used for finding raltive vectors)
	glm::vec3 up_; 		// Up direction vector relative to camera
	glm::vec3 right_; 	// Right direction vector relative to camera
	glm::vec3 target_; 	// Where the camera is looking

	// Updated only when getViewMatrix() is called
	glm::mat4 viewMatrix_;

	// Calculate up/right/forward vectors from pitch/yaw
	void updateVectors();

	// Calculate distance from target from zoomAmount
	void updateRadius();

	// Calculate camera position from pitch/yaw/radius
	void updatePosition();

	// Calculate look at view matrix given necessary variables
	// (I just wrote this to see if I could; I still use it because it's cool)
	glm::mat4 lookAt(glm::vec3 camPosition, glm::vec3 lookDirection, glm::vec3 up);

};

#endif

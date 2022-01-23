/** TimeInfo.h
 * Static class for maintaining time-related values
 * now(): 	Current system time
 * delta(): Time elapsed since last frame
 * fps():	Current frames per second
*/

#ifndef TIME_INFO_H
#define TIME_INFO_H

// Use the last FPS_FRAMES_COUNT frames to calculate current fps
#define FPS_FRAMES_COUNT 25

#include <GLFW/glfw3.h> // glfwGetTime()
#include <cstring> 		// memset()

class Time {
public:

	// Initialize some static values
	static void init();

	// Updates the current time and the time since last call
	// Call every frame
	static void update();

	// Accessor functions
	static double now() { return glfwGetTime(); }
	static double delta() { return Time::delta_; }
	static double fps() { return fps_; }

private:
	Time();	// Hide the default constructor to discourage instantiation

	static double lastFrameTime_;			 // Time last frame was renedered
	static double delta_;					 // Time taken to render last frame
	static double fps_;						 // Current frames per second
	static double deltas_[FPS_FRAMES_COUNT]; // Buffer of previous deltas
	static double deltaSum_;				 // Sum of buffer entries
	static unsigned int deltaIndex_;		 // Current buffer insertion index
};

#endif // TIME_H

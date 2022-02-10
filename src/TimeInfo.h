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

#include <GL/glew.h>
#include <GLFW/glfw3.h> // glfwGetTime()
#include <cstring> 		// memset()

class Time {
public:

	// Initialize some static values
	static void init();

	// Updates the current time and the time since last call
	static void update();

	// Returns true if another step needs to be taken
	static bool takeNextStep();

	// Accessor functions
	static double now() { return glfwGetTime(); }
	static double delta() { return Time::updateDelta_; }
	static double fps() { return fps_; }
	static double timeStep() { return timeStep_; }
	
	static double lastUpdateTime() { return lastUpdateTime_; }
	static double programStartTime() { return programStartTime_; }

private:
	Time();	// Hide the default constructor to discourage instantiation

	static double lastUpdateTime_;			 // Time last frame was renedered
	static double updateDelta_;		         // Time between last two update()s
	static double fps_;						 // Current frames per second
	static double deltas_[FPS_FRAMES_COUNT]; // Buffer of previous deltas
	static double deltaSum_;				 // Sum of buffer entries
	static unsigned int deltaIndex_;		 // Current buffer insertion index
	static double timeStep_;
	static double lag_;
	static double programStartTime_;
};

#endif // TIME_H

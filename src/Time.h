/** Time.h
 * Static class for accessing time-related values
 * now(): Current system time
 * delta():   Time elapsed since last frame
*/

#ifndef TIME_H
#define TIME_H

#include <GLFW/glfw3.h> // glfwGetTime()

class Time {
public:

	// Updates the current time and the time since last call
	static void computeTimeDelta() {
		Time::delta_ = glfwGetTime() - Time::lastFrameTime;
		Time::lastFrameTime = glfwGetTime();
	}

	static double now() { return glfwGetTime(); }
	static double delta() { return Time::delta_; }

private:
	// Hide the default constructor to discourage instantiation
	Time();

	static double lastFrameTime;
	static double delta_;
};

#endif // TIME_H

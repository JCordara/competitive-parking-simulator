#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Common.h"
#include "Event.h"


// Functor for deleting a GLFW window.
//
// This is used as a custom deleter with std::unique_ptr so that the window
// is properly destroyed when std::unique_ptr needs to clean up its resource
struct WindowDeleter {
	void operator() (GLFWwindow* window) const {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow(window);
	}
};


// Main class for creating and interacting with a GLFW window.
// Only wraps the most fundamental parts of the API
class Window {

public:

	Window(
		int width, 
		int height, 
		const char* title, 
		GLFWmonitor* monitor = NULL, 
		GLFWwindow* share = NULL
	);

	static inline void defaultWindowSizeCallback(
		GLFWwindow* window, int width, int height) { 
			glViewport(0, 0, width, height);
			Events::WindowResized.broadcast(width, height);
	}

	glm::ivec2 getPos() const;
	glm::ivec2 getSize() const;

	int getX() const { return getPos().x; }
	int getY() const { return getPos().y; }
	
	int getWidth() const { return getSize().x; }
	int getHeight() const { return getSize().y; }

	bool isFullScreen();
	void setFullScreen(int monitor);
	int numberOfMonitors();
	int getCurrentMonitorNumber();

	int shouldClose() { return glfwWindowShouldClose(window.get()); }
	void makeContextCurrent() { glfwMakeContextCurrent(window.get()); }
	void swapBuffers() { glfwSwapBuffers(window.get()); }
	void resetViewport() { glViewport(0, 0, getWidth(), getHeight());}

	void setUserPtr(void* data) { glfwSetWindowUserPointer(window.get(), data); }
	void* getUserPtr() { return glfwGetWindowUserPointer(window.get()); }
	operator GLFWwindow*();

private:
	std::unique_ptr<GLFWwindow, WindowDeleter> window; // owning ptr (from GLFW)
};

#endif // WINDOW_H

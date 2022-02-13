#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Common.h"
#include "Event.h"
#include "Window.h"

/* Defines callback functions for mouse and keyboard inputs */
/* Manages keyboard and mouse state */
/* Also manages connecting controllers at runtime */
class Callbacks {
public:
	
	/* You can define the default keybindings here */
	Callbacks();

	void keyCallback(int key, int scancode, int action, int mods);
	void mouseButtonCallback(int button, int action, int mods);
	void cursorPosCallback(double xpos, double ypos);
	void scrollCallback(double xoffset, double yoffset);

	/* Binds a void event to a key */
	void bindKey(int key, int action, Event<void>* event);

	/* Helper functions for data management */
	unsigned int createKey(int a, int b);
	void updatePressedKeys(int key, int action);
	void updateMouseDelta(double x, double y);

	/* Other callbacks */
	static void errorCallback(int error, const char* desc);
	static void gamepadCallback(int gid, int event);

private:

	/* Mapping of keys to events */
	std::unordered_map<int, Event<void>*> keyEvents;

	/* Keep track of all pressed keys at any given time */
	std::unordered_set<int> pressedKeys;

	/* Keep track of current mouse position and frame delta */
	double mouseDelta[2] = {0.0, 0.0};
	double mousePos[2]   = {0.0, 0.0};
};

/* Connects callback functions and manages connected gamepads and their input */
/* Provides an interface for binding keys to void events */
class InputManager {
public:
	InputManager(std::shared_ptr<Window> w);

	void processInput();
	void processGamepadInput();

	void bindKey(int key, int action, Event<void>* event);
	inline void rebindKey(int key, int action, Event<void>* event) {
		bindKey(key, action, event);
	} // Alias for code clarity

	static int gamepads[GLFW_JOYSTICK_LAST + 1];
	static int gamepadCount;

private:
	std::shared_ptr<Callbacks> callbacks;
	std::shared_ptr<Window> pWindow;

	GLFWgamepadstate state, lastState;
};

#endif // INPUT_MANAGER_H

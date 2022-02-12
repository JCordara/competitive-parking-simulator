#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Common.h"
#include "Event.h"
#include "Window.h"


class Callbacks {
public:

	Callbacks();

	void keyCallback(int key, int scancode, int action, int mods);
	void mouseButtonCallback(int button, int action, int mods);
	void cursorPosCallback(double xpos, double ypos);
	void scrollCallback(double xoffset, double yoffset);

	unsigned int createKey(int a, int b);
	void updatePressedKeys(int key, int action);
	void updateMouseDelta(double x, double y);

private:

	std::unordered_map<int, Event<void>*> keyEvents;
	std::unordered_set<int> pressedKeys;

	double mouseDelta[2] = {0.0, 0.0};
	double mousePos[2] = {0.0, 0.0};
};

class InputManager {
public:
	InputManager(std::shared_ptr<Window> w);
private:
	std::shared_ptr<Callbacks> callbacks;
	std::shared_ptr<Window> pWindow;
};

#endif // INPUT_MANAGER_H

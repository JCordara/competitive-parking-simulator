#include "InputManager.h"
#include "Window.h"

// Called once every frame when a key is pressed
void Callbacks::keyCallback(int key, int scancode, int action, int mods) {
	// Broadcast corresponding event
	Event<void>* event = keyEvents[createKey(key, action)];
	if (event == nullptr) return;
	event->broadcast();	
}

// Called once every frame when a mouse button is pressed
void Callbacks::mouseButtonCallback(int button, int action, int mods) {

}

// Called once every frame when the mouse moves
void Callbacks::cursorPosCallback(double xpos, double ypos) {
	// Move camera with either mouse button
	if (pressedKeys.count(GLFW_MOUSE_BUTTON_1) != 0 ||
		pressedKeys.count(GLFW_MOUSE_BUTTON_2) != 0)
		Events::CameraRotate.broadcast(mouseDelta[0], mouseDelta[1]);
}

// Called once every tick when the scroll wheel scrolls
void Callbacks::scrollCallback(double xoffset, double yoffset) {
	Events::CameraZoom.broadcast(xoffset, yoffset);
}


// Assign keys to events
Callbacks::Callbacks() {
	keyEvents[createKey(GLFW_KEY_W, GLFW_PRESS)] = &Events::PlayerAccelerate;
	keyEvents[createKey(GLFW_KEY_W, GLFW_RELEASE)] = &Events::PlayerAccelerateRelease;
	keyEvents[createKey(GLFW_KEY_S, GLFW_PRESS)] = &Events::PlayerReverse;
	keyEvents[createKey(GLFW_KEY_S, GLFW_RELEASE)] = &Events::PlayerReverseRelease;
	keyEvents[createKey(GLFW_KEY_A, GLFW_PRESS)] = &Events::PlayerTurnLeft;
	keyEvents[createKey(GLFW_KEY_A, GLFW_RELEASE)] = &Events::PlayerTurnLeftRelease;
	keyEvents[createKey(GLFW_KEY_D, GLFW_PRESS)] = &Events::PlayerTurnRight;
	keyEvents[createKey(GLFW_KEY_D, GLFW_RELEASE)] = &Events::PlayerTurnRightRelease;
	keyEvents[createKey(GLFW_KEY_SPACE, GLFW_PRESS)] = &Events::PlayerBrake;
	keyEvents[createKey(GLFW_KEY_SPACE, GLFW_RELEASE)] = &Events::PlayerBrakeRelease;
	keyEvents[createKey(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS)] = &Events::PlayerHandbrake;
	keyEvents[createKey(GLFW_KEY_LEFT_SHIFT, GLFW_RELEASE)] = &Events::PlayerHandbrakeRelease;
}



unsigned int Callbacks::createKey(int a, int b) {
	return (b << 8) + a;
}

InputManager::InputManager(std::shared_ptr<Window> w) 
	: callbacks(std::make_shared<Callbacks>())
	, pWindow(w)
{

	pWindow->setUserPtr(callbacks.get());

	glfwSetKeyCallback(*pWindow, [](GLFWwindow* w, int k, int s, int a, int m) -> void {
		Callbacks* cb = static_cast<Callbacks*>(glfwGetWindowUserPointer(w));
		// Maintain set of pressed keys
		cb->updatePressedKeys(k, a);
		// Ignore held key repeating
		if (a == GLFW_REPEAT) return;

		cb->keyCallback(k, s, a, m);
	});

	glfwSetMouseButtonCallback(*pWindow, [](GLFWwindow* w, int b, int a, int m) {
		Callbacks* cb = static_cast<Callbacks*>(glfwGetWindowUserPointer(w));
		// Maintain set of pressed keys (including mouse buttons)
		cb->updatePressedKeys(b, a);

		cb->mouseButtonCallback(b, a, m);
	});

	glfwSetCursorPosCallback(*pWindow, [](GLFWwindow* w, double x, double y) {
		Callbacks* cb = static_cast<Callbacks*>(glfwGetWindowUserPointer(w));
		// Update mouse position and delta values for this frame
		cb->updateMouseDelta(x, y);

		cb->cursorPosCallback(x, y);
	});

	glfwSetScrollCallback(*pWindow, [](GLFWwindow* w, double x, double y) {
		Callbacks* cb = static_cast<Callbacks*>(glfwGetWindowUserPointer(w));
		cb->scrollCallback(x, y);
	});
}

void Callbacks::updatePressedKeys(int key, int action) {
	if (action == GLFW_PRESS)
		pressedKeys.insert(key);
	else if (action ==GLFW_RELEASE) 
		pressedKeys.erase(key);
}

void Callbacks::updateMouseDelta(double x, double y) {
	mouseDelta[0] = x - mousePos[0];
	mouseDelta[1] = y - mousePos[1];
	mousePos[0] = x;
	mousePos[1] = y;
}

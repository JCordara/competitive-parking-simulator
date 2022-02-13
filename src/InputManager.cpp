#include "InputManager.h"
#include "Window.h"

// Called once every frame when a key is pressed
void Callbacks::keyCallback(int key, int scancode, int action, int mods) {
	// Broadcast corresponding event if bound
	Event<void>* event = keyEvents[createKey(key, action)];
	if (event != nullptr)
		event->broadcast();

	if(key == GLFW_KEY_W || key == GLFW_KEY_S) {
		float forward  = static_cast<float>(pressedKeys.count(GLFW_KEY_W));
		float backward = static_cast<float>(pressedKeys.count(GLFW_KEY_S));
		Events::PlayerAccelerate.broadcast(forward - backward);
	}

	if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
		float right = static_cast<float>(pressedKeys.count(GLFW_KEY_D));
		float left  = static_cast<float>(pressedKeys.count(GLFW_KEY_A));
		Events::PlayerSteer.broadcast(right - left);
	}

	if(key == GLFW_KEY_SPACE) {
		float v = static_cast<float>(pressedKeys.count(GLFW_KEY_SPACE));
		Events::PlayerBrake.broadcast(v);
	}

	if (key == GLFW_KEY_LEFT_SHIFT) {
		float v = static_cast<float>(pressedKeys.count(GLFW_KEY_LEFT_SHIFT));
		Events::PlayerHandbrake.broadcast(v);
	}

}

// Called once every frame when a mouse button is pressed
void Callbacks::mouseButtonCallback(int button, int action, int mods) {
	// Nothing here yet
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
	//bindKey(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS, &Events::PlayerHandbrake);
}


void Callbacks::bindKey(int key, int action, Event<void>* event) {
	keyEvents[createKey(key, action)] = event;
}

unsigned int Callbacks::createKey(int a, int b) {
	return (b << 8) + a;
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


/* Other callbacks */
void Callbacks::errorCallback(int error, const char* desc) {
	// Log::error(desc);
}

void Callbacks::gamepadCallback(int gid, int event) {
	if (event == GLFW_CONNECTED)
        InputManager::gamepads[InputManager::gamepadCount++] = gid;
    else if (event == GLFW_DISCONNECTED)
    {
        int i;

        for (i = 0;  i < InputManager::gamepadCount;  i++)
        {
            if (InputManager::gamepads[i] == gid)
                break;
        }

        for (i = i + 1;  i < InputManager::gamepadCount;  i++)
            InputManager::gamepads[i - 1] = InputManager::gamepads[i];

        InputManager::gamepadCount--;
    }
}

/* Define static variables out of the way*/
int InputManager::gamepads[GLFW_JOYSTICK_LAST + 1] = {};
int InputManager::gamepadCount = 0;



InputManager::InputManager(std::shared_ptr<Window> w) 
	: callbacks(std::make_shared<Callbacks>())
	, pWindow(w)
{

	// Initialize connected gamepad data
	memset(gamepads, 0, sizeof(gamepads));

	for (int gid = GLFW_JOYSTICK_1;  gid <= GLFW_JOYSTICK_LAST;  gid++)
    {
        if (glfwJoystickPresent(gid))
            gamepads[gamepadCount++] = gid;
    }

	glfwSetJoystickCallback(&Callbacks::gamepadCallback);

	// Pass the callback class through the glfw window for lambda access
	pWindow->setUserPtr(callbacks.get());

	/* Attach callbacks to glfw and pre-process inputs */
	glfwSetKeyCallback(*pWindow, [](GLFWwindow* w, int k, int s, int a, int m) {
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

	glfwSetErrorCallback(&Callbacks::errorCallback);
}

void InputManager::processInput() {
	glfwPollEvents();

	if (gamepadCount != 0) {
		processGamepadInput();
	}
}

/* This function definitely needs some work still */
void InputManager::processGamepadInput() {
	// Loop through connected controllers
	for (int i = 0; i < gamepadCount; i++) {
		// Get their current state
		glfwGetGamepadState(gamepads[i], &state);
		// Loop through axes
		for (int i = 0; i < sizeof(state.axes); i++) {
			switch(i){
				case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER:
					// If state has changed
					if (state.axes[i] != lastState.axes[i]) {
						// Create axis from the two triggers
						float vr = 0.5f * (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1);
						float vl = -0.5f * (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1);
						// Broadcast the acceleration event using that axis
						Events::PlayerAccelerate.broadcast(vr + vl);
					}
					break;
				case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER:
					if (state.axes[i] != lastState.axes[i]) {
						float vr = 0.5f * (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1);
						float vl = -0.5f * (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1);
						Events::PlayerAccelerate.broadcast(vr + vl);
					}
					break;
				// Left joystick x axis
				case GLFW_GAMEPAD_AXIS_LEFT_X:
					// Deadzone [-0.1, 0.1] = 0.0
					if (state.axes[i] <= 0.1f && state.axes[i] >= -0.1f) 
						state.axes[i] = 0.0f;

					// If state has changed
					if (state.axes[i] != lastState.axes[i]) {
						// Broadcast steer direction
						float v = state.axes[i];
						Events::PlayerSteer.broadcast(v);
					}
					break;
			}
		}
		// Loop through buttons
		for (int i = 0; i < sizeof(state.buttons); i++) {
			switch(i) {
				case GLFW_GAMEPAD_BUTTON_SQUARE:
					// If button's pressed state has changed
					if (state.buttons[i] != lastState.buttons[i]) {
						// Broadcast handbrake event
						Events::PlayerHandbrake.broadcast(state.buttons[i]);
					}
					break;
			}
		}
		// Update last state for next iteration
		lastState = state;
	}
}

void InputManager::bindKey(int key, int action, Event<void>* event) {
	callbacks->bindKey(key, action, event);
}

#include "InputSystem.h"
#include "Window.h"

/* Define static variables out of the way*/
int InputSystem::gamepads[GLFW_JOYSTICK_LAST + 1] = {};
int InputSystem::gamepadCount = 0;


/* Initialize input devices and connect callbacks */
InputSystem::InputSystem(std::shared_ptr<Window> w) 
	: callbacks(std::make_shared<Callbacks>())
	, pWindow(w)
{

	// Initialize connected gamepad data
	memset(gamepads, 0, sizeof(gamepads));

	// Check for connected gamepads
	for (int gid = GLFW_JOYSTICK_1;  gid <= GLFW_JOYSTICK_LAST;  gid++)
    {
        if (glfwJoystickPresent(gid)) 
            gamepads[gamepadCount++] = gid;
		
		// Set lastState to nonsense values
		memset(lastState[gid].axes, 2, sizeof(lastState[gid].axes));
		memset(lastState[gid].buttons, -1, sizeof(lastState[gid].buttons));
    }

	// Setup callback for gamepad connects and disconnects at runtime
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

	glfwSetMouseButtonCallback(*pWindow, [](GLFWwindow* w, int b, int a, int m){
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

	// Setup error callback
	glfwSetErrorCallback(&Callbacks::errorCallback);

	Events::ControllerComponentInit.registerHandler<InputSystem, 
		&InputSystem::registerControllerComponent>(this);

}

/* Respond to inputs from the last frame */
void InputSystem::processInput() {
	// Invoke all input callbacks this frame
	glfwPollEvents();

	// Update current state of active control axes
	callbacks->updateControlAxes();

	// Process controller inputs for all connected controllers
	if (gamepadCount != 0) {
		processGamepadInput();
	}
}

/* Preprocess controller inputs and invoke appriopriate callbacks */
void InputSystem::processGamepadInput() {
	
	// Iterate through connected controllers
	for (int g = 0; g < gamepadCount; g++) {
		
		// Get their current state
		glfwGetGamepadState(gamepads[g], &state);
		
		// Iterate through axes
		for (int i = 0; i < static_cast<int>(numAxes); i++) {
			// Set axis deadzones
			// Joysticks are always jittering in the range [-0.05, 0.05]
			if (state.axes[i] <= 0.1f && state.axes[i] >= -0.1f) 
				state.axes[i] = 0.0f;
			
			// If an axis' state has changed, invoke the callback
			if (state.axes[i] != lastState[g].axes[i]) {
				callbacks->gamepadAxisCallback(state.axes, i, numAxes);
			}
		}

		// Loop through buttons
		for (int i = 0; i < static_cast<int>(numButtons); i++) {
			// If button's pressed state has changed, invoke the callback
			if (state.buttons[i] != lastState[g].buttons[i]) {
				callbacks->gamepadButtonCallback(state.buttons, i, numButtons);
			}
		}

		// Update last state of this controller for next iteration
		lastState[g] = state;
	}
}


void InputSystem::bindInput(int input, int action, Event<void>* event) {
	callbacks->bindInput(input, action, event);
}

void InputSystem::bindInput(int input, Event<float>* event) {
	callbacks->bindInput(input, event);
}

void InputSystem::bindInput(int input, Event<shared_ptr<Entity>, float>* event, shared_ptr<Entity> e) {
	callbacks->bindInput(input, event, e);
}

void InputSystem::createAxis(
	int inputPositive,
	int inputNegative,
	Event<float>* event,
	ControlAxis::TypeEnum type)
{
	callbacks->createAxis(inputPositive, inputNegative, event, type);
}

/* Create an axis between two inputs and bind to event */
void InputSystem::createAxis(
	int inputPositive, 
	int inputNegative, 
	Event<shared_ptr<Entity>, float>* event, 
	shared_ptr<Entity> e,
	ControlAxis::TypeEnum type) 
{
	callbacks->createAxis(inputPositive, inputNegative, event, e, type);
}


/* Create an axis from a single axis input and binds to event */
void InputSystem::createAxis(int input, Event<float>* event) {
	callbacks->createAxis(input, -1, event, ControlAxis::SINGLE);
}

void InputSystem::createAxis(int input, Event<shared_ptr<Entity>, float>* event, shared_ptr<Entity> e) {
	callbacks->createAxis(input, -1, event, e, ControlAxis::SINGLE);
}

void InputSystem::removeAxis(int inputA, int inputB) {
	callbacks->removeAxis(inputA, inputB);
}

void InputSystem::registerControllerComponent(ControllerComponent& component) {
	component.setInputSystem(
		dynamic_pointer_cast<InputSystem>(shared_from_this()));
}

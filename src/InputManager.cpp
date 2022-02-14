#include "InputManager.h"
#include "Window.h"

// Called once per tick when a key is pressed
void Callbacks::keyCallback(int key, int scancode, int action, int mods) {
	// Broadcast void events bound to this key
	auto voidEvent = voidKeyEvents[createMapKey(key, action)];
	if (voidEvent != nullptr)
		voidEvent->broadcast();
	 
	// Broadcast float events bound to this key
	auto floatEvent = floatKeyEvents[key];
	if (floatEvent != nullptr)
		floatEvent->broadcast(static_cast<float>(action));

	// Update axes bound to this key
	auto controlAxis = controlAxes[key];
	if (controlAxis) controlAxis->setInputValue(key, static_cast<float>(action));

}

// Called once per tick when a mouse button is pressed
void Callbacks::mouseButtonCallback(int button, int action, int mods) {
	// Nobody's home
}

// Called once per tick when the mouse moves
void Callbacks::cursorPosCallback(double xpos, double ypos) {
	// Move camera with either mouse button
	if (pressedKeys.count(GLFW_MOUSE_BUTTON_1) != 0 ||
		pressedKeys.count(GLFW_MOUSE_BUTTON_2) != 0)
		Events::CameraRotate.broadcast(mouseDelta[0], mouseDelta[1]);
}

// Called once per tick when the scroll wheel scrolls
void Callbacks::scrollCallback(double xoffset, double yoffset) {
	Events::CameraZoom.broadcast(xoffset, yoffset);
}

// Called once per tick when any controller button is pressed
void Callbacks::gamepadButtonCallback(
	unsigned char buttons[], int button, unsigned int n) {

	// Broadcast void events bound to this key
	auto voidEvent = voidKeyEvents[createMapKey(button, buttons[button])];
	if (voidEvent != nullptr)
		voidEvent->broadcast();
	 
	// Broadcast float events bound to this key
	auto floatEvent = floatKeyEvents[button];
	if (floatEvent != nullptr)
		floatEvent->broadcast(static_cast<float>(buttons[button]));

	// Update axes bound to this key
	auto controlAxis = controlAxes[button];
	if (controlAxis) {
		float v = static_cast<float>(buttons[button]);
		controlAxis->setInputValue(button, v);
	}
	
}

// Called once per tick when any controller axis is changed
void Callbacks::gamepadAxisCallback(float axes[], int axis, unsigned int n) {

	// Update axes bound to this axis
	auto controlAxis = controlAxes[axis];
	if (controlAxis) controlAxis->setInputValue(axis, axes[axis]);

}

// Assign keys to events
Callbacks::Callbacks() {
	// Could set default values in here if we want, probs wont tho
}


void Callbacks::createAxis(
	int inputPositive, 
	int inputNegative, 
	Event<float>* event, 
	ControlAxis::TypeEnum type) 
{
	// Make a pointer to a new axis
	std::shared_ptr<ControlAxis> axis = std::make_shared<ControlAxis>(
		inputPositive, inputNegative, event, type);

	// Map both keys to the same pointer
	// Wish I had a multimap
	// Trying to figure out how to use the boost library, apparently it has one
	controlAxes.insert({inputPositive, axis});

	if (type != ControlAxis::SINGLE)
		controlAxes.insert({inputNegative, axis});
}

void Callbacks::removeAxis(int inputA, int inputB) {
	controlAxes.erase(inputA);
	controlAxes.erase(inputB);
}

unsigned int Callbacks::createMapKey(int a, int b) {
	return (b << 8) + a;
}

void Callbacks::updatePressedKeys(int key, int action) {
	if (action == GLFW_PRESS)
		pressedKeys.insert(key);
	else if (action == GLFW_RELEASE) 
		pressedKeys.erase(key);
}

void Callbacks::updateMouseDelta(double x, double y) {
	mouseDelta[0] = x - mousePos[0];
	mouseDelta[1] = y - mousePos[1];
	mousePos[0] = x;
	mousePos[1] = y;
}

void Callbacks::updateControlAxes() {
	// The map of axes contains duplicates (both inputs mapped to same axis)
	// Need to extract the axes from the map into a set to remove duplicates
	// A multi-index map would be nice here
	std::unordered_set<std::shared_ptr<ControlAxis>> axes;

	// Insert axes into the set
	for (auto& a : controlAxes) 
		if (a.second != nullptr) axes.insert(a.second);

	// Update each axis in the set
	for (auto& a : axes)
		a->update();
}

void Callbacks::bindInput(int input, int action, Event<void>* event) {
	voidKeyEvents[createMapKey(input, action)] = event;
}

void Callbacks::bindInput(int input, Event<float>* event) {
	floatKeyEvents[input] = event;
}


/* Other callbacks */
void Callbacks::errorCallback(int error, const char* desc) {
	// Log::error(desc);
}

/* Keep track of controllers as they connect and disconnect */
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


/* Initialize input devices and connect callbacks */
InputManager::InputManager(std::shared_ptr<Window> w) 
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

}

/* Respond to inputs from the last frame */
void InputManager::processInput() {
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
void InputManager::processGamepadInput() {
	
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


void InputManager::bindInput(int input, int action, Event<void>* event) {
	callbacks->bindInput(input, action, event);
}

void InputManager::bindInput(int input, Event<float>* event) {
	callbacks->bindInput(input, event);
}

void InputManager::createAxis(
	int inputPositive,
	int inputNegative,
	Event<float>* event,
	ControlAxis::TypeEnum type)
{
	callbacks->createAxis(inputPositive, inputNegative, event, type);
}

/* Create an axis from a single axis input and binds to event */
void InputManager::createAxis(int input, Event<float>* event) {
	callbacks->createAxis(input, -1, event, ControlAxis::SINGLE);
}

void InputManager::removeAxis(int inputA, int inputB) {
	callbacks->removeAxis(inputA, inputB);
}


/* Control Axes implementations */
ControlAxis::ControlAxis(
	int inputA, int inputB, Event<float>* event, TypeEnum type) 
	: value_(0.0f)
	, event_(event) 
	, inputA_(inputA)
	, inputB_(inputB)
	, valueA_(0.0f)
	, valueB_(0.0f)
	, type_(type)
{
	if (type_ == AXIS) {
		valueA_ = -1.0f;
		valueB_ = -1.0f;
	}
}

ControlAxis::ControlAxis(int inputA, int inputB, TypeEnum type)
	: ControlAxis(inputA, inputB, nullptr, type) {}

void ControlAxis::bindEvent(Event<float>* event) { event_ = event; }


void ControlAxis::setInputValue(int input, float value) {
	
	// Normlize axis values
	if (type_ == AXIS) value = (value + 1) * 0.5f;

	if      (input == inputA_) valueA_ = value;
	else if (input == inputB_) valueB_ = value;

	update();
}

void ControlAxis::update() {
	// Update the value
	float oldValue = value_;
	value_ = valueA_ - valueB_;
	
	if (type_ == SINGLE) value_ = valueA_;
	
	// If the value has changed, broadcast the event with new value
	// If an event is attached
	if (event_ != nullptr && value_ != oldValue)
		event_->broadcast(value_);
}


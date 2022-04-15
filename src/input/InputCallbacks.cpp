#include "InputSystem.h"

// Called once per tick when a key is pressed
void Callbacks::keyCallback(int key, int scancode, int action, int mods) {

	// Recompile scripts (alt + R)
	if ((mods & GLFW_MOD_ALT) && (key == GLFW_KEY_R) && (action && GLFW_PRESS)) {
		Events::RecompileScripts.broadcast();
	}

	// Broadcast void events bound to this key
	if(voidKeyEvents.count(createMapKey(key, action))) {
		auto voidEvent = voidKeyEvents.at(createMapKey(key, action));
		voidEvent->broadcast();
	}
	
	// Broadcast float events bound to this key
	if (floatKeyEvents.count(key)) {
		auto floatEvent = floatKeyEvents.at(key);
		floatEvent->broadcast(static_cast<float>(action));
	}

	// Broadcast entity & float events bound to this key
	if (entityFloatKeyEvents.count(key)) {
		auto entityFloatEvent = entityFloatKeyEvents.at(key);
		entityFloatEvent.first->broadcast(
			entityFloatEvent.second, 
			static_cast<float>(action)
		);
	}

	// Update axes bound to this key
	if (controlAxes.count(key)) {
		auto controlAxis = controlAxes.at(key);
		controlAxis->setInputValue(key, static_cast<float>(action));
	}
}

// Called once per tick when a mouse button is pressed
void Callbacks::mouseButtonCallback(int button, int action, int mods) {
	// Nobody's home
}

// Called once per tick when the mouse moves
void Callbacks::cursorPosCallback(double xpos, double ypos) {
	// Nobody's home
}

// Called once per tick when the scroll wheel scrolls
void Callbacks::scrollCallback(double xoffset, double yoffset) {
	// Nobody's home
}

// Called once per tick when any controller button is pressed
void Callbacks::gamepadButtonCallback(
	unsigned char buttons[], int button, unsigned int n) {

	// Broadcast void events bound to this key
	if (voidKeyEvents.count(createMapKey(button, buttons[button]))) {
		auto voidEvent = voidKeyEvents.at(createMapKey(button, buttons[button]));
		voidEvent->broadcast();
	}
	 
	// Broadcast float events bound to this key
	if (floatKeyEvents.count(button)) {
		auto floatEvent = floatKeyEvents.at(button);
		floatEvent->broadcast(static_cast<float>(buttons[button]));
	}

	// Broadcast entity & float events bound to this key
	if (entityFloatKeyEvents.count(button)) {
		auto entityFloatEvent = entityFloatKeyEvents.at(button);
		entityFloatEvent.first->broadcast(
			entityFloatEvent.second, 
			static_cast<float>(buttons[button]));
	}

	// Update axes bound to this key
	if (controlAxesButtons.count(button)) {
		auto controlAxis = controlAxesButtons.at(button);
		float v = static_cast<float>(buttons[button]);
		controlAxis->setInputValue(button, v);
	}

	// On DPAD press
	if (buttons[button] > 0.0f) {
		if (button == GLFW_GAMEPAD_BUTTON_DPAD_DOWN || button == GLFW_GAMEPAD_BUTTON_DPAD_LEFT) 
			Events::StickMoveY.broadcast( 1.0f);
		if (button == GLFW_GAMEPAD_BUTTON_DPAD_UP || button == GLFW_GAMEPAD_BUTTON_DPAD_RIGHT)
			Events::StickMoveY.broadcast(-1.0f);
	}
	
}

// Called once per tick when any controller axis is changed
void Callbacks::gamepadAxisCallback(float axes[], int axis, unsigned int n) {

	// Broadcast float events bound to this axis
	if (floatKeyEvents.count(axis)) {	
		auto floatEvent = floatKeyEvents.at(axis);
		floatEvent->broadcast(static_cast<float>(axes[axis]));
	}

	// Update axes bound to this axis
	if (controlAxes.count(axis)) {	
		auto controlAxis = controlAxes.at(axis);
		controlAxis->setInputValue(axis, axes[axis]);
	}

	if (axis == GLFW_GAMEPAD_AXIS_LEFT_Y)      Events::StickMoveY.broadcast(axes[axis]);

}

// Assign keys to events
Callbacks::Callbacks() {
	// Could set default values in here if we want, probs wont tho
	// Sike
	voidKeyEvents[createMapKey(GLFW_GAMEPAD_BUTTON_A, GLFW_PRESS)] = &Events::APressed;
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
	if (type != ControlAxis::KEY) {
		controlAxes.insert({inputPositive, axis});
		if (type != ControlAxis::SINGLE)
			controlAxes.insert({inputNegative, axis});
	} else {
		controlAxesButtons.insert({inputPositive, axis});
		controlAxesButtons.insert({inputNegative, axis});
	}
}

void Callbacks::createAxis(
	int inputPositive, 
	int inputNegative, 
	Event<weak_ptr<Entity>, float>* event, 
	weak_ptr<Entity> e,
	ControlAxis::TypeEnum type) 
{
	// Make a pointer to a new axis
	std::shared_ptr<ControlAxis> axis = std::make_shared<EntityControlAxis>(
		inputPositive, inputNegative, event, e, type);

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
	for (auto& a : axes) a->update();
}

void Callbacks::bindInput(int input, int action, Event<void>* event) {
	voidKeyEvents[createMapKey(input, action)] = event;
}

void Callbacks::bindInput(int input, Event<float>* event) {
	floatKeyEvents[input] = event;
}

void Callbacks::bindInput(int input, Event<weak_ptr<Entity>, float>* event, weak_ptr<Entity> e) {
	entityFloatKeyEvents.try_emplace(input, event, e);
}


/* Other callbacks */
void Callbacks::errorCallback(int error, const char* desc) {
	// Log::error(desc);
}

/* Keep track of controllers as they connect and disconnect */
void Callbacks::gamepadCallback(int gid, int event) {
	if (event == GLFW_CONNECTED)
        InputSystem::gamepads[InputSystem::gamepadCount++] = gid;
    else if (event == GLFW_DISCONNECTED)
    {
        int i;

        for (i = 0;  i < InputSystem::gamepadCount;  i++)
        {
            if (InputSystem::gamepads[i] == gid)
                break;
        }

        for (i = i + 1;  i < InputSystem::gamepadCount;  i++)
            InputSystem::gamepads[i - 1] = InputSystem::gamepads[i];

        InputSystem::gamepadCount--;
    }
}

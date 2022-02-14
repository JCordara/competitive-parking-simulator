#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Common.h"
#include "Event.h"
#include "Window.h"

/* Create an axis between two keys or axes */
class ControlAxis {
public:

	enum TypeEnum {
		KEY,
		AXIS,
		SINGLE
	};

	ControlAxis(int inputA, int inputB, TypeEnum type = KEY);
	ControlAxis(
		int inputA, int inputB, Event<float>* event, TypeEnum type = KEY);

	void bindEvent(Event<float>* event);
	float getValue() { return value_; }
	void setInputValue(int input, float value);
	void setType(TypeEnum type) { type_ = type; }
	void update();


protected:
	// Type (0 = key, 1 = axis) determines how input values are treated
	// Axis inputs get normalized to [0.0, 1.0]
	TypeEnum type_;
	// Inputs
	int inputA_;
	int inputB_;
	// Input values
	float valueA_;
	float valueB_;
	// The resulting value of the axis
	float value_;
	// The attached event that is triggered by this axis
	Event<float>* event_;
};


/* Define callback functions for mouse and keyboard inputs */
/* Manage keyboard and mouse state */
/* Also manage connecting controllers at runtime */
class Callbacks {
public:
	
	Callbacks();

	void keyCallback(int key, int scancode, int action, int mods);
	void mouseButtonCallback(int button, int action, int mods);
	void cursorPosCallback(double xpos, double ypos);
	void scrollCallback(double xoffset, double yoffset);
	void gamepadButtonCallback(
		unsigned char buttons[], int button, unsigned int n);
	void gamepadAxisCallback(float axes[], int axis, unsigned int n);

	/* Other callbacks */
	static void errorCallback(int error, const char* desc);
	// Handle controller connects and disconnects at runtime
	static void gamepadCallback(int gid, int event);

	/* Create an axis between two inputs and bind to event */
	void createAxis(
		int inputPositive, 
		int inputNegative, 
		Event<float>* event, 
		ControlAxis::TypeEnum type = ControlAxis::KEY
	);

	/* Remove an axis bound to the keys */
	void removeAxis(int inputA, int inputB);

	/* Helper functions for data management */
	unsigned int createMapKey(int a, int b);
	void updatePressedKeys(int key, int action);
	void updateMouseDelta(double x, double y);
	void updateControlAxes();

	/* Bind an input (key/button) to a parameterless (void) event */
	void bindInput(int input, int action, Event<void>* event);
	/* Bind a input (key/button) to a float event */
	void bindInput(int input, Event<float>* event);


private:

	/* Mapping of keys to events */
	std::unordered_map<int, Event<void>*> voidKeyEvents;
	std::unordered_map<int, Event<float>*> floatKeyEvents;

	/* Mapping of keys to axes */
	std::unordered_map<int, std::shared_ptr<ControlAxis>> controlAxes;

	/* Keep track of all pressed keys at any given time */
	std::unordered_set<int> pressedKeys;

	/* Keep track of current mouse position and frame delta */
	double mouseDelta[2] = {0.0, 0.0};
	double mousePos[2]   = {0.0, 0.0};
};

/* Connect callback functions and manages connected gamepads and their input */
/* Provide an interface for binding keys to void events */
class InputManager {
public:
	InputManager(std::shared_ptr<Window> w);

	void processInput();
	void processGamepadInput();

	/* Bind an input (key/button) to a parameterless (void) event */
	void bindInput(int input, int action, Event<void>* event);
	/* Bind a input (key/button) to a float event */
	void bindInput(int input, Event<float>* event);

	/* Create an axis between two inputs and bind to an event */
	void createAxis(
		int inputPositive,
		int inputNegative,
		Event<float>* event,
		ControlAxis::TypeEnum type = ControlAxis::KEY
	);

	/* Create an axis from a single axis input and bind to event */
	void createAxis(int inputPositive, Event<float>* event);

	/* Remove an axis bound to the keys */
	void removeAxis(int inputA, int inputB);

	static int gamepads[GLFW_JOYSTICK_LAST + 1];
	static int gamepadCount;

private:
	std::shared_ptr<Callbacks> callbacks;
	std::shared_ptr<Window> pWindow;

	GLFWgamepadstate state, lastState[GLFW_JOYSTICK_LAST + 1];

	// Number of accessible axes and buttons defined in GLFW
	const unsigned int numAxes = 6;
	const unsigned int numButtons = 15;
};

#endif // INPUT_MANAGER_H

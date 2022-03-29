#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GameSystem.h"
#include "ControlAxis.h"
#include "InputCallbacks.h"

#include "Common.h"
#include "Window.h"


/* Connect callback functions and manages connected gamepads and their input */
/* Provide an interface for binding keys to void events */
class InputSystem : public GameSystem {
public:
	InputSystem(std::shared_ptr<Window> w);
	void update() {}

	void processInput();
	void processGamepadInput();

	/* Bind an input (key/button) to a parameterless (void) event */
	void bindInput(int input, int action, Event<void>* event);
	/* Bind a input (key/button) to a float event */
	void bindInput(int input, Event<float>* event);
	/* Bind a input (key/button) to a float event */
	void bindInput(int input, Event<shared_ptr<Entity>, float>* event, shared_ptr<Entity> e);

	/* Create an axis between two inputs and bind to an event */
	void createAxis(
		int inputPositive,
		int inputNegative,
		Event<float>* event,
		ControlAxis::TypeEnum type = ControlAxis::KEY
	);

	void createAxis(
		int inputPositive,
		int inputNegative,
		Event<shared_ptr<Entity>, float>* event,
		shared_ptr<Entity> e,
		ControlAxis::TypeEnum type = ControlAxis::KEY
	);

	/* Create an axis from a single axis input and bind to event */
	void createAxis(int input, Event<float>* event);
	
	/* Create an axis from a single axis input and bind to event */
	void createAxis(int input, Event<shared_ptr<Entity>, float>* event, shared_ptr<Entity> e);

	/* Remove an axis bound to the keys */
	void removeAxis(int inputA, int inputB);

	static int gamepads[GLFW_JOYSTICK_LAST + 1];
	static int gamepadCount;

private:
	std::shared_ptr<Callbacks> callbacks;
	std::shared_ptr<Window> pWindow;

	GLFWgamepadstate state, lastState[GLFW_JOYSTICK_LAST + 1];

	void registerControllerComponent(ControllerComponent& component);

	// Number of accessible axes and buttons defined in GLFW
	const unsigned int numAxes = 6;
	const unsigned int numButtons = 15;

	vector<int> stickAxes;
};

#endif // INPUT_MANAGER_H

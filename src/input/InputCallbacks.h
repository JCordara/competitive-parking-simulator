#ifndef INPUT_CALLBACKS_H
#define INPUT_CALLBACKS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ControlAxis.h"
#include "Event.h"
#include "Common.h"

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
	
	/* Create an axis between two inputs and bind to event */
	void createAxis(
		int inputPositive, 
		int inputNegative, 
		Event<shared_ptr<Entity>, float>* event, 
		shared_ptr<Entity> e,
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
	/* Bind a input (key/button) to a Entity, float event */
	void bindInput(int input, Event<shared_ptr<Entity>, float>* event, shared_ptr<Entity> e);


private:

	/* Mapping of keys to events */
	std::unordered_map<int, Event<void>*> voidKeyEvents;
	std::unordered_map<int, Event<float>*> floatKeyEvents;
	std::unordered_map<int, 
		std::pair<Event<shared_ptr<Entity>, float>*, shared_ptr<Entity>>> entityFloatKeyEvents;

	/* Mapping of keys to axes */
	std::unordered_map<int, std::shared_ptr<ControlAxis>> controlAxes;

	/* Keep track of all pressed keys at any given time */
	std::unordered_set<int> pressedKeys;

	/* Keep track of current mouse position and frame delta */
	double mouseDelta[2] = {0.0, 0.0};
	double mousePos[2]   = {0.0, 0.0};
};

#endif // INPUT_CALLBACKS_H

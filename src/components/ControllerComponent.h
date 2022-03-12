#ifndef CONTROLLER_COMPONENT_H
#define CONTROLLER_COMPONENT_H

#include "Components.h"
#include "Input/ControlAxis.h"

class InputSystem;

class ControllerComponent : public BaseComponent {
public:
    ControllerComponent(shared_ptr<Entity> parent);
    ~ControllerComponent();
    static ComponentEnum getType();

	void setInputSystem(shared_ptr<InputSystem> system) {
		inputSystem = system;
	}

    /* Create an axis between two inputs and bind to event */
	void createAxis(
		int inputPositive, 
		int inputNegative, 
		Event<shared_ptr<Entity>, float>* event, 
		ControlAxis::TypeEnum type = ControlAxis::KEY
	);

	void createAxis(
		int input, 
		Event<shared_ptr<Entity>, float>* event
	);

	/* Remove an axis bound to the keys */
	void removeAxis(int inputA, int inputB);

	/* Bind a input (key/button) to a float event */
	void bindInput(int input, Event<shared_ptr<Entity>, float>* event);

private:
    shared_ptr<InputSystem> inputSystem;

};

#endif // CONTROLLER_COMPONENT_H

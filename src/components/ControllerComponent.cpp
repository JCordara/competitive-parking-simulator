#include "ControllerComponent.h"
#include "Input/InputSystem.h"


ControllerComponent::ControllerComponent(shared_ptr<Entity> parent) 
    : BaseComponent(parent)
{
    Events::ControllerComponentInit.broadcast(*this);
}

ComponentEnum ControllerComponent::getType() {
    return ComponentEnum::controller;
}


/* Create an axis between two inputs and bind to event */
void ControllerComponent::createAxis(
    int inputPositive, 
    int inputNegative, 
    Event<shared_ptr<Entity>, float>* event, 
    ControlAxis::TypeEnum type) 
{
    inputSystem->createAxis(inputPositive, inputNegative, event, entity, type);
}

/* Create an axis from input and bind to event */
void ControllerComponent::createAxis(
    int input, 
    Event<shared_ptr<Entity>, float>* event) 
{
    inputSystem->createAxis(input, event, entity);
}

/* Remove an axis bound to the keys */
void ControllerComponent::removeAxis(int inputA, int inputB) {
    inputSystem->removeAxis(inputA, inputB);
}


/* Bind a input (key/button) to a float event */
void ControllerComponent::bindInput(int input, Event<shared_ptr<Entity>, float>* event) {
    inputSystem->bindInput(input, event, entity);
}

ControllerComponent::~ControllerComponent() {
    // Nothing to do here yet
}

#include "ControllerComponent.h"
#include "InputSystem.h"


ControllerComponent::ControllerComponent(Entity& parent) 
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
    Event<Entity&, float>* event, 
    ControlAxis::TypeEnum type) 
{
    inputSystem->createAxis(inputPositive, inputNegative, event, entity, type);
}

/* Remove an axis bound to the keys */
void ControllerComponent::removeAxis(int inputA, int inputB) {
    inputSystem->removeAxis(inputA, inputB);
}


/* Bind a input (key/button) to a float event */
void ControllerComponent::bindInput(int input, Event<Entity&, float>* event) {
    inputSystem->bindInput(input, event, entity);
}

ControllerComponent::~ControllerComponent() {
    // Nothing to do here yet
}

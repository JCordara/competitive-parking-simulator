#include "PhysicsComponent.h"


PhysicsComponent::PhysicsComponent(Entity& parent) 
    : BaseComponent(parent)
{}

ComponentEnum PhysicsComponent::getType() {
    return ComponentEnum::physics;
}

PhysicsComponent::~PhysicsComponent() {
    // Nothing to do here yet
}

#include "PhysicsComponent.h"

ComponentEnum PhysicsComponent::getType() {
    return Component::physics;
}

PhysicsComponent::~PhysicsComponent() {
    // Nothing to do here yet
}

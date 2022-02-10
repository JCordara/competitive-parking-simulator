#include "PhysicsComponent.h"

ComponentEnum PhysicsComponent::getType() {
    return ComponentEnum::physics;
}

PhysicsComponent::~PhysicsComponent() {
    // Nothing to do here yet
}

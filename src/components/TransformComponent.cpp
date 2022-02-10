#include "TransformComponent.h"

TransformComponent::TransformComponent() 
    : x(0)
    , y(0)
    , z(0)
{}

ComponentEnum TransformComponent::getType() {
    return ComponentEnum::transform;
}

TransformComponent::~TransformComponent() {
    // Nothing to do here yet
}

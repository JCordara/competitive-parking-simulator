#include "TransformComponent.h"

TransformComponent::TransformComponent(Entity& parent) 
    : BaseComponent(parent)
    , x(0)
    , y(0)
    , z(0)
{}

ComponentEnum TransformComponent::getType() {
    return ComponentEnum::transform;
}

TransformComponent::~TransformComponent() {
    // Nothing to do here yet
}

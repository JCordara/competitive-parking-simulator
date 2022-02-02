#include "Component.h"


TransformComponent::TransformComponent() 
    : x(0)
    , y(0)
    , z(0)
{}

ComponentEnum TransformComponent::getType() {
    return Component::transform;
}

TransformComponent::~TransformComponent() {
    // Nothing to do here yet
}

// BaseComponent destructor has to be defined even though it's a pure virtual 
// function ¯\_(ツ)_/¯ go figure
BaseComponent::~BaseComponent() {
    // Nothing to do here either
}

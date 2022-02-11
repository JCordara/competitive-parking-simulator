#include "ModelComponent.h"


ModelComponent::ModelComponent(Entity& parent) 
    : BaseComponent(parent)
{}

ComponentEnum ModelComponent::getType() {
    return ComponentEnum::model;
}

ModelComponent::~ModelComponent() {
    // Nothing to do here yet
}

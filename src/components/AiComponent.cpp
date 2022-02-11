#include "AiComponent.h"


AiComponent::AiComponent(Entity& parent) 
    : BaseComponent(parent)
{}

ComponentEnum AiComponent::getType() {
    return ComponentEnum::ai;
}

AiComponent::~AiComponent() {
    // Nothing to do here yet
}

#include "RendererComponent.h"


RendererComponent::RendererComponent(Entity& parent) 
    : BaseComponent(parent)
{}

ComponentEnum RendererComponent::getType() {
    return ComponentEnum::renderer;
}

RendererComponent::~RendererComponent() {
    // Nothing to do here yet
}

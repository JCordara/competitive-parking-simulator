#include "AudioComponent.h"


AudioComponent::AudioComponent(Entity& parent) 
    : BaseComponent(parent)
{}

ComponentEnum AudioComponent::getType() {
    return ComponentEnum::audio;
}

AudioComponent::~AudioComponent() {
    // Nothing to do here yet
}

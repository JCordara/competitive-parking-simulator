#include "AudioComponent.h"

ComponentEnum AudioComponent::getType() {
    return Component::audio;
}

AudioComponent::~AudioComponent() {
    // Nothing to do here yet
}

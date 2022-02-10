#include "AudioComponent.h"

ComponentEnum AudioComponent::getType() {
    return ComponentEnum::audio;
}

AudioComponent::~AudioComponent() {
    // Nothing to do here yet
}

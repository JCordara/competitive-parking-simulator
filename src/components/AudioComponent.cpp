#include "AudioComponent.h"
#include "audio/AudioSystem.h"


AudioComponent::AudioComponent(Entity& parent) 
    : BaseComponent(parent)
{
    // Get reference to AudioSystem
    Events::AudioComponentInit.broadcast(*this);

    transform = entity.getComponent<TransformComponent>();

    source = &audioSystem->createSource(transform->getGlobalPosition());

    sounds.push_back(&audioSystem->loadAudio("audio/oof.wav"));

    Events::CarBoxCollision.registerHandler<AudioComponent,
        &AudioComponent::onCrash>(this);
}

ComponentEnum AudioComponent::getType() {
    return ComponentEnum::audio;
}

AudioComponent::~AudioComponent() {
    // Nothing to do here yet
}


void AudioComponent::playSound(int id) {
    if (id >= sounds.size()) return;
    source->playAudio(*sounds[id]);
}


void AudioComponent::updatePosition(glm::vec3 newPos) {
    source->setPosition(newPos);
}


#include "AudioComponent.h"
#include "Audio/AudioSystem.h"


AudioComponent::AudioComponent(weak_ptr<Entity> parent):
    BaseComponent(parent), 
    calculateEngineSound("scripts/calculateEngineSound.lua"),
    enginePitch(1.0f),
    engineGain(1.0f),
    engineSpeed(0.0f)
{
    // Get reference to AudioSystem
    Events::AudioComponentInit.broadcast(*this);

    transform = getEntity()->getComponent<TransformComponent>();

    source = audioSystem->createSource(transform->getGlobalPosition());

}

ComponentEnum AudioComponent::getType() {
    return ComponentEnum::audio;
}

AudioComponent::~AudioComponent() {
    // Nothing to do here yet
}


void AudioComponent::playSound(AudioTrigger t) {
    if (!sounds.count(t)) return;
    source->playAudio(sounds[t]);
}

void AudioComponent::playSoundVaried(AudioTrigger t) {
    if (!sounds.count(t)) return;
    source->setPitch(Random::randomFloat(0.9f, 1.1f));
    source->setGain(Random::randomFloat(0.7f, 1.0f));
    source->playAudio(sounds[t]);
}

void AudioComponent::addSound(AudioTrigger t, const std::string soundFile) {
    sounds.insert( {t, audioSystem->loadAudio(soundFile)} );
}

void AudioComponent::playEngineSound() {
    if (!hasEngineSound()) return;
    engineSpeed = vehicle.lock()->getSpeed();
    calculateEngineSound.run();
    engineSource->setGain(engineGain);
    engineSource->setPitch(enginePitch);
    if (!engineSource->isPlaying()) engineSource->playAudio(engineSound);
}

void AudioComponent::addEngineSound(const std::string soundFile, weak_ptr<VehicleComponent> vehicle) {
    engineSource = audioSystem->createSource();
    engineSource->setLooping(true);
    this->vehicle = vehicle;
    engineSound = audioSystem->loadAudio(soundFile);
    calculateEngineSound.capture(enginePitch);
    calculateEngineSound.capture(engineGain);
    calculateEngineSound.capture(engineSpeed);
}


void AudioComponent::updatePosition(glm::vec3 newPos) {
    source->setPosition(newPos);
    if (engineSource != nullptr) engineSource->setPosition(newPos);
}

bool AudioComponent::isStatic() { 
    return source->isStatic(); 
}

void AudioComponent::setStatic(bool value) { 
    source->setStatic(value); 
}

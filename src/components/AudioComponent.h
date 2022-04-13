#ifndef AUDIO_COMPONENT_H
#define AUDIO_COMPONENT_H

#include "Components.h"
#include "LuaScript.h"

class AudioSystem;
class AudioSource;
class Audio;

enum class AudioTrigger {
    Collision,
	Park
};

class AudioComponent : public BaseComponent {
public:

    AudioComponent(weak_ptr<Entity> parent);
    ~AudioComponent();
    static ComponentEnum getType();

    void playSound(AudioTrigger t);
    void playSoundVaried(AudioTrigger t);
    void addSound(AudioTrigger t, const std::string soundFile);

    void playEngineSound();
    void addEngineSound(const std::string soundFile, weak_ptr<VehicleComponent> vehicle);
    bool hasEngineSound() { return engineSource != nullptr; }

    void updatePosition(glm::vec3 newPos);
    
    bool isStatic();
    void setStatic(bool value);

    void setAudioSystem(sp<AudioSystem> as) {
        audioSystem = as;
    }

private:

    sp<AudioSystem> audioSystem;
    sp<TransformComponent> transform;

    unordered_map<AudioTrigger, shared_ptr<Audio>> sounds;
    shared_ptr<AudioSource> source;

    shared_ptr<AudioSource> engineSource;
    shared_ptr<Audio> engineSound;
	weak_ptr<VehicleComponent> vehicle;
    Script calculateEngineSound;
    float enginePitch, engineGain, engineSpeed;
};

#endif // AUDIO_COMPONENT_H

#ifndef AUDIO_COMPONENT_H
#define AUDIO_COMPONENT_H

#include "Components.h"

class AudioSystem;
class AudioSource;
class Audio;

class AudioComponent : public BaseComponent {
public:
    AudioComponent(Entity& parent);
    ~AudioComponent();
    static ComponentEnum getType();

    void setAudioSystem(sp<AudioSystem> as) {
        audioSystem = as;
    }

    void updatePosition(glm::vec3 newPos);

    void playSound(int id);

    void onCrash();

private:

    sp<AudioSystem> audioSystem;
    sp<TransformComponent> transform;

    std::vector<Audio*> sounds;
    AudioSource* source;
};

#endif // AUDIO_COMPONENT_H

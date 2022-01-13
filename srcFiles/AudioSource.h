#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#include "AL/efx.h"

#include "glm/glm.hpp"

#include "Audio.h"
#include "OpenALError.h"

#include <iostream>

class AudioSource {

public:

    AudioSource();
    AudioSource(const glm::vec3 position_init);
    AudioSource& operator=(const AudioSource& other);
    ~AudioSource();

    void playAudio(const Audio& audio);
    bool isPlaying();

    void setPosition(const glm::vec3 pos);
    void setVelocity(const glm::vec3 vel);
    glm::vec3 position() const;
    glm::vec3 velocity() const;

    void setGain(const float g);

    void regenerateSource();

private:

    glm::vec3 position_;
    glm::vec3 velocity_;

    ALuint sourceID;
    float pitch;
    float gain;
    bool looping;
    
    Audio currAudio;

};


#endif // AUDIO_SOURCE_H

#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "AudioSettings.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Time.h"

#include "Audio.h"
#include "OpenALError.h"

#include <iostream>

class AudioSource {

public:

    AudioSource(bool staticSource = false);
    AudioSource(const glm::vec3 position_init, bool staticSource = false);
    AudioSource& operator=(const AudioSource& other);
    ~AudioSource();

    void playAudio(const Audio& audio);
    bool isPlaying();

    void setPosition(const glm::vec3& _position);
    //void setVelocity(const glm::vec3 vel);
    glm::vec3 position() const;
    glm::vec3 velocity() const;

    void setGain(const float g);

    void regenerateSource();

    operator ALuint() const { return sourceID; }

private:

    glm::vec3 position_;
    glm::vec3 velocity_;

    ALuint sourceID;
    float pitch;
    float gain;
    bool looping;
    bool _isStatic;
    
    const Audio* currAudio;
    Audio _null;

};


#endif // AUDIO_SOURCE_H

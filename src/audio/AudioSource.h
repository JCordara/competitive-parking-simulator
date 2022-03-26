#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "AudioSettings.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common.h"
#include "TimeInfo.h"

#include "Audio.h"
#include "OpenALError.h"

#include <iostream>

class AudioSource {

public:

    AudioSource(bool staticSource = false);
    AudioSource(const glm::vec3 position_init, bool staticSource = false);
    AudioSource& operator=(const AudioSource& other);
    ~AudioSource();

    void playAudio(shared_ptr<Audio> audio);
    bool isPlaying();
    bool isStatic() { return isStatic_; }
    void setStatic(bool value) { isStatic_ = value; }

    void setPosition(const glm::vec3& position);
    //void setVelocity(const glm::vec3 vel);
    glm::vec3 position() const;
    glm::vec3 velocity() const;

    void setGain(const float g);
    void setPitch(const float p);
    void setLooping(const bool l);

    void regenerateSource();

    operator ALuint() const { return sourceID; }

private:

    glm::vec3 position_;
    glm::vec3 velocity_;

    ALuint sourceID;
    float pitch;
    float gain;
    bool looping;
    bool isStatic_;
    
    shared_ptr<Audio> currAudio;

};


#endif // AUDIO_SOURCE_H

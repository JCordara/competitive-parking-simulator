#include "AudioSource.h"

AudioSource::AudioSource() 
    : sourceID(0)
    , pitch(1.0f)
    , gain(1.0f)
    , looping(false)
    , currAudio()
    , position_(glm::vec3())
    , velocity_(glm::vec3())
{
    // Initialize source properties
    alGenSources(1, &sourceID);
    alSourcef(sourceID, AL_PITCH, pitch);
    alSourcef(sourceID, AL_GAIN, gain);
    alSource3f(sourceID, AL_POSITION, position_.x, position_.y, position_.z);
    alSource3f(sourceID, AL_VELOCITY, velocity_.x, velocity_.y, velocity_.z);
    alSourcei(sourceID, AL_LOOPING, looping);
    alSourcei(sourceID, AL_BUFFER, currAudio.id());
}

AudioSource::AudioSource(const glm::vec3 position_init) : AudioSource() {
      position_ = position_init;
      alSource3f(sourceID, AL_POSITION, position_.x, position_.y, position_.z);
}


void AudioSource::playAudio(const Audio& audio) {

    // Dont switch buffers if unnecessary
    if (audio.id() != currAudio.id()) {
        currAudio = audio;
        alSourcei(sourceID, AL_BUFFER, currAudio.id());
    }

    alSourcePlay(sourceID);
}

bool AudioSource::isPlaying() {
    ALint state;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING)
        return true;
    else
        return false;
}


void AudioSource::setPosition(const glm::vec3 pos) {
    position_ = pos;
    alSource3f(sourceID, AL_POSITION, position_.x, position_.y, position_.z);
}

void AudioSource::setVelocity(const glm::vec3 vel) {
    velocity_ = vel;
    alSource3f(sourceID, AL_VELOCITY, velocity_.x, velocity_.y, velocity_.z);
}

glm::vec3 AudioSource::position() const {
    return position_;
}

glm::vec3 AudioSource::velocity() const {
    return velocity_;
}

void AudioSource::setGain(const float g) {
    gain = g;
    alSourcef(sourceID, AL_GAIN, gain);
}



void AudioSource::regenerateSource() {
    alGenSources(1, &sourceID);
}

AudioSource& AudioSource::operator=(const AudioSource& other) {
    if (&other != this) {
        alGenSources(1, &sourceID);
        pitch = other.pitch;
        gain = other.gain;
        looping = other.looping;
        currAudio = other.currAudio;
        position_ = other.position_;
        velocity_ = other.velocity_;
    }
    return *this;
}

AudioSource::~AudioSource() {
    alDeleteSources(1, &sourceID);
}


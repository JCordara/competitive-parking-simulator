#include "AudioSource.h"

AudioSource::AudioSource(bool staticSource) 
    : sourceID(0)
    , pitch(1.0f)
    , gain(1.0f)
    , looping(false)
    , position_(glm::vec3())
    , velocity_(glm::vec3())
    , _null()
    , currAudio(&_null)
    , _isStatic(staticSource)
{

    // Initialize source properties
    alGenSources(1, &sourceID);
    alSourcef(sourceID, AL_PITCH, pitch);
    alSourcef(sourceID, AL_GAIN, gain);
    alSource3f(sourceID, AL_POSITION, position_.x, position_.y, position_.z);
    alSource3f(sourceID, AL_VELOCITY, velocity_.x, velocity_.y, velocity_.z);
    alSourcei(sourceID, AL_LOOPING, looping);
    alSourcei(sourceID, AL_BUFFER, *currAudio);
}

AudioSource::AudioSource(const glm::vec3 position_init, bool staticSource) 
    : AudioSource() 
{
      position_ = position_init;
      alSource3f(sourceID, AL_POSITION, position_.x, position_.y, position_.z);
}


void AudioSource::playAudio(const Audio& audio) {

    if (isPlaying()) 
        alSourceStop(sourceID);

    // Dont switch buffers if unnecessary
    if (audio != *currAudio) {
        currAudio = &audio;
        alSourcei(sourceID, AL_BUFFER, *currAudio);
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


void AudioSource::setPosition(const glm::vec3& _position) {

    // Update non-static source velocity each time this function is called
    // Expected that the position of non-static sources is updated per-frame
    if (!_isStatic) {
        // Get last known position
        glm::vec3 lastPosition;
        alGetSourcefv(sourceID, AL_POSITION, glm::value_ptr(lastPosition));
        
        // Predict instantaneous velocity given last frame exec time
        glm::vec3 velocity(
            (_position.x - lastPosition.x) / static_cast<float>(Time::delta()),
            (_position.y - lastPosition.y) / static_cast<float>(Time::delta()),
            (_position.z - lastPosition.z) / static_cast<float>(Time::delta())
        );
        
        // Scale down doppler effect intensity
        velocity *= DOPPLER_SCALE_FACTOR;

        alSource3f(sourceID, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    alSourcefv(sourceID, AL_POSITION, glm::value_ptr(_position));
    
}

#if 0
void AudioSource::setVelocity(const glm::vec3 vel) {
    velocity_ = vel;
    alSource3f(sourceID, AL_VELOCITY, velocity_.x, velocity_.y, velocity_.z);
}
#endif

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

void AudioSource::setPitch(const float p) {
    pitch = p;
    alSourcef(sourceID, AL_PITCH, pitch);
}

void AudioSource::setLooping(const bool l) {
    alSourcef(sourceID, AL_LOOPING, (l ? AL_TRUE : AL_FALSE));
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
    debug_log("Entering AudioSource destructor");
    if (alIsSource(sourceID)) alDeleteSources(1, &sourceID);
    debug_log("Leaving AudioSource destructor");
}


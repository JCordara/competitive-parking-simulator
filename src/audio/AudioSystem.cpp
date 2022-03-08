
#include "AudioSystem.h"


/** Open default audio device and initialize OpenAL context */
AudioSystem::AudioSystem() {

    // Get devices
    std::vector<std::string> deviceNames = getDeviceNames();
    audioDevices.reserve(5); // Ensure sufficient space for most machines
    for (auto& name : deviceNames) {
        audioDevices.push_back(std::make_shared<AudioDevice>(name));
    }
    
    // Treat first device listed as default device
    currentDevice = 0;
    audioDevices[currentDevice]->open();

    // Create context
    p_alcContext = alcCreateContext(*audioDevices[currentDevice], nullptr);
    if (!p_alcContext) {
        std::cerr << "Failed to set sound context\n";
        exit(1);
    }

    // Set as current context
    if (!alcMakeContextCurrent(p_alcContext)) {
        std::cerr << "Failed to make context current\n";
        exit(1);
    }

    // Register self with audio components when they are created
    Events::AudioComponentInit.registerHandler<AudioSystem,
        &AudioSystem::registerAudioComponent>(this);

    // Hacky stuff
    engineSound = &loadAudio("audio/engine.wav");
    carSource = &createSource();
    carSource->setPitch(1.0f);
    carSource->setGain(0.075f);

    music = &loadAudio("audio/CoconutMall.wav");
    musicPlayer = &createStaticSource(glm::vec3());
    musicPlayer->setLooping(true);
    musicPlayer->setGain(0.5f);

    ding = &loadAudio("audio/ding.wav");
    oof = &loadAudio("audio/oof.wav");
    aux = &createSource();
    aux->setGain(1.0f);


    Events::GameStart.registerHandler<AudioSystem,
        &AudioSystem::onGameStart>(this);

    Events::CarParked.registerHandler<AudioSystem,
        &AudioSystem::playDing>(this);

    Events::CarBoxCollision.registerHandler<AudioSystem,
        &AudioSystem::playOof>(this);
}

void AudioSystem::update() {

    glm::mat4 m = listener->getGlobalMatrix();
    glm::vec3 front = glm::normalize(glm::vec3(-m * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::vec3(m * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

    setListenerOrientation(front, up);
    setListenerPosition(m[3]);

    float speed = car->getLinearVelocity().magnitude();
    enginePitch = speed * 0.28f + 0.95f;
    engineGain  = speed * 0.008f + 0.076f;
    carSource->setPosition(m[3]);
    carSource->setPitch(enginePitch);
    carSource->setGain(engineGain);

    aux->setPosition(m[3]);
}

void AudioSystem::onGameStart() {
    musicPlayer->playAudio(*music);
    startEngine();
}

void AudioSystem::startEngine() {
    carSource->setLooping(true);
    carSource->playAudio(*engineSound);
}

void AudioSystem::stopEngine() {
    carSource->setLooping(false);
}

void AudioSystem::playDing(Entity&) {
    aux->setPitch(Random::randomFloat(0.9f, 1.1f));
    aux->setGain(1.0f);
    aux->playAudio(*ding);
}

void AudioSystem::playOof() {
    aux->setPitch(Random::randomFloat(0.9f, 1.1f));
    aux->setGain(Random::randomFloat(0.6f, 0.9f));
    aux->playAudio(*oof);
}



Audio& AudioSystem::loadAudio(std::string filepath) {
    audioBuffers.push_back(std::make_shared<Audio>(filepath));
    filepaths.push_back(filepath);
    return *audioBuffers.back();
}

AudioSource& AudioSystem::createSource() {
    audioSources.push_back(std::make_shared<AudioSource>());
    return *audioSources.back();
}

AudioSource& AudioSystem::createSource(const glm::vec3 position_init) {
    audioSources.push_back(std::make_shared<AudioSource>(position_init));
    return *audioSources.back();
}

AudioSource& AudioSystem::createStaticSource() {
    audioSources.push_back(std::make_shared<AudioSource>(true));
    return *audioSources.back();
}

AudioSource& AudioSystem::createStaticSource(const glm::vec3 position_init) {
    audioSources.push_back(std::make_shared<AudioSource>(position_init, true));
    return *audioSources.back();
}

void AudioSystem::setListenerPosition(const glm::vec3& _position) {
    // Automatically calculate listener velocity
    // This is possible when this function is called every frame
    
    // Get last known position
    glm::vec3 lastPosition;
    alGetListenerfv(AL_POSITION, glm::value_ptr(lastPosition));

    // Predict instantaneous velocity given last frame exec time
    glm::vec3 velocity(
        (_position.x - lastPosition.x) / static_cast<float>(Time::delta()),
        (_position.y - lastPosition.y) / static_cast<float>(Time::delta()),
        (_position.z - lastPosition.z) / static_cast<float>(Time::delta())
    );

    // Scale down doppler effect intensity
    velocity *= DOPPLER_SCALE_FACTOR;

    alListenerfv(AL_POSITION, glm::value_ptr(_position));
    alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
}

void AudioSystem::setListenerOrientation(const glm::vec3& _front, const glm::vec3& _up) {
    // Combine into a single float array for OpenAL
    float ori[6] = {
        -_front.x, -_front.y, -_front.z,
            _up.x,     _up.y,     _up.z
    };
    
    alListenerfv(AL_ORIENTATION, ori);
}

void AudioSystem::setListenerOrientation(const glm::mat4& _vm) {
    this->setListenerOrientation(
        glm::vec3(_vm[0][2], _vm[1][2], _vm[2][2]), 
        glm::vec3(_vm[0][1], _vm[1][1], _vm[2][1])
    );
}


int AudioSystem::availableDevicesCount() const {
    return static_cast<int>(audioDevices.size());
}

/** Retrieve a list of string representations of available devices */
std::vector<std::string> AudioSystem::getDeviceNames() {

    // Get list of device names as null-terminated strings
    const ALCchar* str = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);

    const char* p = str;
    std::vector<std::string> devices;

    do {
        devices.push_back(std::string(p));
        p += devices.back().size() + 1;
    } while (*(p) != '\0');

    return devices;
}

AudioDevice& AudioSystem::getDevice(int deviceID) {
    if (deviceID == -1)
        return *audioDevices[currentDevice];
    else
        return *audioDevices[deviceID];
}

/** Opening a new device requires destroying and recreating devices and contexts */
void AudioSystem::openDevice(int deviceID) {
    // Do nothing if the device isn't changing
    if (deviceID == currentDevice) return;

    // Close any/all devices
    for (auto& device : audioDevices) device->close();

    // Unset audio context
    if (!alcMakeContextCurrent(nullptr)) {
        std::cerr << "Failed to unset context\n";
    }

    alcDestroyContext(p_alcContext);

    // Clear all but audioSource vectors
    audioBuffers.clear();
    audioDevices.clear();

    // Get devices
    std::vector<std::string> deviceNames = getDeviceNames();
    for (auto& name : deviceNames) {
        audioDevices.push_back(std::make_shared<AudioDevice>(name));
    }
    
    currentDevice = deviceID;
    audioDevices[currentDevice]->open();

    // Create context
    p_alcContext = alcCreateContext(*audioDevices[currentDevice], nullptr);
    if (!p_alcContext) {
        std::cerr << "Failed to set sound context\n";
        exit(1);
    }
    
    // Set as current context
    if (!alcMakeContextCurrent(p_alcContext)) {
        std::cerr << "Failed to make context current\n";
        exit(1);
    }
    
    // Reload all sounds back into new buffers
    for (auto& filepath : filepaths) {
        audioBuffers.push_back(std::make_shared<Audio>(filepath));
    }
    
    // Regenerate source IDs (possible memory leak, pretty minimal though)
    for (auto& source : audioSources) {
        source->regenerateSource();
    }

    alCheckErrors();

}


void AudioSystem::registerAudioComponent(AudioComponent& component) {
    component.setAudioSystem(
        dynamic_pointer_cast<AudioSystem>(shared_from_this()));
}



AudioSystem::~AudioSystem() {
    debug_log("Entering AudioSystem destructor");
    // Delete sources and buffers
    audioSources.clear();
    audioBuffers.clear();

    // Unset audio context
    if (!alcMakeContextCurrent(nullptr)) {
        std::cerr << "Failed to unset context\n";
    }

    alcDestroyContext(p_alcContext);

    audioDevices.clear();

    
    debug_log("Leaving AudioSystem destructor");
}

#include "AudioSystem.h"


/** Open default audio device and initialize OpenAL context */
AudioSystem::AudioSystem(shared_ptr<Scene> scene): scene(scene) {

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

    // Setup music player
    musicPlayer = createStaticSource();
    musicPlayer->setGain(0.1f);
    musicPlayer->setLooping(true);
    music = loadAudio("audio/CoconutMall.wav");
	menuMusic = loadAudio("audio/tempMenu.wav"); //Need to change

    // Register self with audio components when they are created
    Events::AudioComponentInit.registerHandler<AudioSystem,
        &AudioSystem::registerAudioComponent>(this);

    Events::NewGame.registerHandler<AudioSystem,
        &AudioSystem::onGameStart>(this);

	Events::SetMenuMusic.registerHandler<AudioSystem,
		&AudioSystem::onMenuStart>(this);

    Events::Collision.registerHandler<AudioSystem,
        &AudioSystem::onCollision>(this);

	Events::CarParked_Official.registerHandler<AudioSystem,
		&AudioSystem::onPark>(this);

    Events::ChangeMusicVolume.registerHandler<AudioSystem,
        &AudioSystem::onMusicVolumeChanged>(this);

}

void AudioSystem::update() {
    auto l = listener.lock(); if (!l) return;
    // Update listener position
    glm::mat4 m = l->getGlobalMatrix();
    glm::vec3 front = glm::normalize(glm::vec3(-m * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::vec3(m * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

    setListenerOrientation(front, up);
    setListenerPosition(m[3]);

    // Loop through audio components in the scene
    for (auto wp_c : scene->iterate<AudioComponent>()) {
        auto c = wp_c.lock(); if(!c) continue;
        if (!c->isStatic()) {
            auto transform = c->getEntity()->getComponent<TransformComponent>();
            c->updatePosition(transform->getGlobalPosition());
            if (c->hasEngineSound()) c->playEngineSound();
        }
    }

    // printf("%s, (%.2f, %.2f, %.2f)\n", (musicPlayer->isPlaying() ? "true" : "false"), musicPlayer->position().x, musicPlayer->position().y, musicPlayer->position().z);
}

void AudioSystem::onGameStart() {
    musicPlayer->playAudio(music);
}

void AudioSystem::onMenuStart() {
	musicPlayer->playAudio(menuMusic);
}

void AudioSystem::onCollision(weak_ptr<Entity> wpe0, weak_ptr<Entity> wpe1) {
    auto e0 = wpe0.lock(); auto e1 = wpe1.lock();
    if (!e0 || !e1) return;

    if (auto c = e0->getComponent<AudioComponent>())
        c->playSoundVaried(AudioTrigger::Collision);
    if (auto c = e1->getComponent<AudioComponent>())
        c->playSoundVaried(AudioTrigger::Collision);
}

void AudioSystem::onPark(weak_ptr<Entity> wpe0) {
	auto e0 = wpe0.lock();
	if (!e0) return;

	if (auto c = e0->getComponent<AudioComponent>())
		c->playSoundVaried(AudioTrigger::Park);
}

void AudioSystem::onMusicVolumeChanged(float gain) {
    musicPlayer->setGain(gain);
}


shared_ptr<Audio> AudioSystem::loadAudio(std::string filepath) {
    audioBuffers.push_back(std::make_shared<Audio>(filepath));
    filepaths.push_back(filepath);
    return audioBuffers.back();
}

shared_ptr<AudioSource> AudioSystem::createSource() {
    audioSources.push_back(std::make_shared<AudioSource>());
    return audioSources.back();
}

shared_ptr<AudioSource> AudioSystem::createSource(const glm::vec3 position_init) {
    audioSources.push_back(std::make_shared<AudioSource>(position_init));
    return audioSources.back();
}

shared_ptr<AudioSource> AudioSystem::createStaticSource() {
    audioSources.push_back(std::make_shared<AudioSource>(true));
    return audioSources.back();
}

shared_ptr<AudioSource> AudioSystem::createStaticSource(const glm::vec3 position_init) {
    audioSources.push_back(std::make_shared<AudioSource>(position_init, true));
    return audioSources.back();
}

void AudioSystem::setListenerPosition(const glm::vec3& _position) {
    // Automatically calculate listener velocity
    // This is only possible when this function is called every frame
    
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



int AudioSystem::availableDevicesCount() const {
    return static_cast<int>(audioDevices.size());
}

/* Retrieve a list of string representations of available devices */
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

shared_ptr<AudioDevice> AudioSystem::getDevice(int deviceID) {
    if (deviceID == -1)
        return audioDevices[currentDevice];
    else
        return audioDevices[deviceID];
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

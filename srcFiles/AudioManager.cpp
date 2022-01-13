
#include "AudioManager.h"

AudioManager& AudioManager::instance() {
    static AudioManager instance_;
    return instance_;
}

/** Open default audio device and initialize OpenAL context */
AudioManager::AudioManager() {

    // Get devices
    std::vector<std::string> deviceNames = getDeviceNames();
    audioDevices.reserve(5); // Ensure sufficient space for most machines
    for (auto& name : deviceNames) {
        audioDevices.emplace_back(name);
    }
    
    // Treat first device listed as default device
    currentDevice = 0;
    audioDevices[currentDevice].open();

    // Create context
    p_alcContext = alcCreateContext(audioDevices[currentDevice], nullptr);
    if (!p_alcContext) {
        std::cerr << "Failed to set sound context\n";
        exit(1);
    }

    // Set as current context
    if (!alcMakeContextCurrent(p_alcContext)) {
        std::cerr << "Failed to make context current\n";
        exit(1);
    }
}

AudioManager::~AudioManager() {
    // Close any/all devices
    for (auto& device : audioDevices) device.close();

    // Unset audio context
    if (!alcMakeContextCurrent(nullptr)) {
        std::cerr << "Failed to unset context\n";
    }

    alcDestroyContext(p_alcContext);
}


Audio& AudioManager::loadAudio(std::string filepath) {
    audioBuffers.emplace_back(filepath);
    filepaths.push_back(filepath);
    return audioBuffers.back();
}

AudioSource& AudioManager::createSource() {
    audioSources.emplace_back();
    return audioSources.back();
}

AudioSource& AudioManager::createSource(const glm::vec3 position_init) {
    audioSources.emplace_back(position_init);
    return audioSources.back();
}


int AudioManager::availableDevicesCount() const {
    return static_cast<int>(audioDevices.size());
}

/** Retrieve a list of string representations of available devices */
std::vector<std::string> AudioManager::getDeviceNames() {

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

AudioDevice& AudioManager::getDevice(int deviceID) {
    if (deviceID == -1)
        return audioDevices[currentDevice];
    else
        return audioDevices[deviceID];
}

/** Opening a new device requires destroying and recreating devices and contexts */
void AudioManager::openDevice(int deviceID) {
    // Do nothing if the device isn't changing
    if (deviceID == currentDevice) return;

    // Close any/all devices
    for (auto& device : audioDevices) device.close();

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
        audioDevices.emplace_back(name);
    }
    
    currentDevice = deviceID;
    audioDevices[currentDevice].open();

    // Create context
    p_alcContext = alcCreateContext(audioDevices[currentDevice], nullptr);
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
        audioBuffers.emplace_back(filepath);
    }
    
    // Regenerate source IDs (possible memory leak, pretty minimal though)
    for (auto& source : audioSources) {
        source.regenerateSource();
    }

    alCheckErrors();

}
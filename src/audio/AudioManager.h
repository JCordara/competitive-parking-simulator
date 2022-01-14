#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#define AL_LIBTYPE_STATIC

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

#include <glm/glm.hpp>

#include "AudioDevice.h"
#include "AudioSource.h"
#include "Audio.h"

#include <memory>

class AudioManager {

public:
    // Static reference singleton implementation
    static AudioManager& instance();

    Audio& loadAudio(std::string filepath);

    AudioSource& createSource();
    AudioSource& createSource(const glm::vec3 position_init);

    // ----- Device management methods -----
    int availableDevicesCount() const;
    void openDevice(int deviceID);
    std::vector<std::string> getDeviceNames();
    AudioDevice& getDevice(int deviceID);

    // ----- Rule of 5 Singleton deletions -----
    AudioManager(const AudioManager&) = delete;
    AudioManager(AudioManager&&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    AudioManager& operator=(AudioManager&&) = delete;
    
    // Public destructor
    ~AudioManager();

private:

    std::vector<Audio> audioBuffers;
    std::vector<AudioSource> audioSources;
    std::vector<AudioDevice> audioDevices;

    uint8_t currentDevice;
    ALCcontext* p_alcContext;

    std::vector<std::string> filepaths;

    AudioManager(); // Private ctor to disallow multiple instantiation

};


#endif // AUDIO_MANAGER_H

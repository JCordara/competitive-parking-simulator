#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "AudioSettings.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Time.h"

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

    AudioSource& createStaticSource();
    AudioSource& createStaticSource(const glm::vec3 position_init);

    void setListenerPosition(const glm::vec3& _position);
    void setListenerOrientation(const glm::vec3& _front, const glm::vec3& _up);
    
    // Listener orientation can also be set using the camera's view matrix
    void setListenerOrientation(const glm::mat4& _vm);

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

    std::vector<std::shared_ptr<Audio>> audioBuffers;
    std::vector<std::shared_ptr<AudioSource>> audioSources;
    std::vector<std::shared_ptr<AudioDevice>> audioDevices;

    uint8_t currentDevice;
    ALCcontext* p_alcContext;

    std::vector<std::string> filepaths;

    AudioManager(); // Private ctor to disallow multiple instantiation

};


#endif // AUDIO_MANAGER_H

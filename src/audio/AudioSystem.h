#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

// Includes global settings for audio like doppler intensity, debug logs, etc
#include "AudioSettings.h"

// Necessary OpenAL includes
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

// For managing 3D space and time
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "TimeInfo.h"

// Audio system includes
#include "AudioDevice.h"
#include "AudioSource.h"
#include "Audio.h"

#include "GameSystem.h"

#include <memory>   // Smart pointers
#include <iostream> // Degubbing

#include <LuaScript.h>


class AudioComponent;


class AudioSystem : public GameSystem {

public:

    AudioSystem();
    void update();

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
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem(AudioSystem&&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;
    AudioSystem& operator=(AudioSystem&&) = delete;
    
    void registerAudioComponent(AudioComponent& component);

    void setListener(sp<TransformComponent> tc) { listener = tc; }

    void onGameStart();

    void startEngine();
    void stopEngine();

    void playDing(Entity&);
    void playOof(glm::vec3&);

    // Public destructor
    ~AudioSystem();

    PxRigidDynamic* car;
    float enginePitch;
    float engineGain;
    float speed;

    Script calculateEngineSound;

private:

    std::vector<std::shared_ptr<Audio>> audioBuffers;
    std::vector<std::shared_ptr<AudioSource>> audioSources;
    std::vector<std::shared_ptr<AudioDevice>> audioDevices;

    uint8_t currentDevice;
    ALCcontext* p_alcContext;

    std::vector<std::string> filepaths;

    shared_ptr<TransformComponent> listener;

    AudioSource* carSource;
    Audio* engineSound;

    AudioSource* musicPlayer;
    Audio* music;

    AudioSource* aux;
    Audio* ding;
    Audio* oof;
};


#endif // AUDIO_SYSTEM_H

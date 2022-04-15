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
#include "Event.h"

#include <memory>   // Smart pointers
#include <iostream> // Degubbing

#include <LuaScript.h>


class AudioComponent;


class AudioSystem : public GameSystem {

public:

    AudioSystem(shared_ptr<Scene> scene);
    void update();

    shared_ptr<Audio> loadAudio(std::string filepath);

    shared_ptr<AudioSource> createSource();
    shared_ptr<AudioSource> createSource(const glm::vec3 position_init);

    shared_ptr<AudioSource> createStaticSource();
    shared_ptr<AudioSource> createStaticSource(const glm::vec3 position_init);

    void setListener(sp<TransformComponent> tc) { listener = tc; }
    void setListenerPosition(const glm::vec3& _position);
    void setListenerOrientation(const glm::vec3& _front, const glm::vec3& _up);

    void registerAudioComponent(AudioComponent& component);

    // ----- Device management methods -----
    int availableDevicesCount() const;
    void openDevice(int deviceID);
    std::vector<std::string> getDeviceNames();
    shared_ptr<AudioDevice> getDevice(int deviceID);
    

    // Public destructor
    ~AudioSystem();

    void onGameStart();
	void onMenuStart();
    void onCollision(weak_ptr<Entity> e0, weak_ptr<Entity> s1);
	void onPark(weak_ptr<Entity> wpe0);
    void onMusicVolumeChanged(float gain);
    void onMenuMove();
    void onMenuSelect();

	float getCurrentVolume();

private:

    std::vector<std::shared_ptr<Audio>> audioBuffers;
    std::vector<std::shared_ptr<AudioSource>> audioSources;
    std::vector<std::shared_ptr<AudioDevice>> audioDevices;

    uint8_t currentDevice;
    ALCcontext* p_alcContext;

    std::vector<std::string> filepaths;

    shared_ptr<Scene> scene;
    weak_ptr<TransformComponent> listener;

	float currentVolume = 0.1f;
    shared_ptr<AudioSource> musicPlayer;
    shared_ptr<Audio> music;
	shared_ptr<Audio> menuMusic;

    shared_ptr<AudioSource> auxSource;
    shared_ptr<Audio> menuSelect;
    shared_ptr<Audio> menuClick;

};


#endif // AUDIO_SYSTEM_H

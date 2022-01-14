#ifndef AUDIO_DEVICE_H
#define AUDIO_DEVICE_H

#define AL_LIBTYPE_STATIC

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

#include "OpenALError.h"

#include <vector>
#include <string>
#include <iostream>
#include <memory>

class AudioDevice {
public:

    AudioDevice(std::string deviceName);
    ~AudioDevice();

    void open();
    void close();

    AudioDevice(AudioDevice& other);
    AudioDevice(AudioDevice&& rvalue) noexcept;

    std::string name() const;
    operator ALCdevice*() const;

private:

    ALCdevice* p_alcDevice;
    std::string name_;

    bool isOpen;

};

#endif // AUDIO_DEVICE_H

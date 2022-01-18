#include "AudioDevice.h"

AudioDevice::AudioDevice(std::string deviceName) 
    : name_(deviceName)
    , p_alcDevice(nullptr)
    , isOpen(false)
{}

AudioDevice::AudioDevice(AudioDevice& other) 
    : p_alcDevice(other.p_alcDevice)
    , name_(other.name_)
    , isOpen(other.isOpen)
{
    other.p_alcDevice = nullptr;
    other.isOpen = false;
}

AudioDevice::AudioDevice(AudioDevice&& rvalue) noexcept 
    : p_alcDevice(rvalue.p_alcDevice)
    , name_(rvalue.name_)
    , isOpen(rvalue.isOpen)
{
    rvalue.p_alcDevice = nullptr;
    rvalue.isOpen = false;
}

void AudioDevice::open() {
    p_alcDevice = alcOpenDevice(name_.c_str());

    alcCheckErrors(p_alcDevice);

    if (!p_alcDevice) {
        std::cerr << "Failed to open sound device \"" << name_ << "\"\n";
        exit(1);
    }

    isOpen = true;
}

void AudioDevice::close() {
    // If device hasn't been successfully opened yet, return
    if (!p_alcDevice) return;
    if (!isOpen) return; // Return is device isnt flagged as open

    if (!alcCloseDevice(p_alcDevice)) {
        std::cerr << "Failed to close sound device \"" << name_ << "\"\n";
    }
    isOpen = false;
}

std::string AudioDevice::name() const {
    return name_;
}

AudioDevice::operator ALCdevice*() const {
    return p_alcDevice;
}

AudioDevice::~AudioDevice() {
    debug_log("Entering AudioDevice destructor");
    // If device hasn't been successfully opened yet, return
    if (!p_alcDevice) return;
    if (!isOpen) return; // Return is device isnt flagged as open
    
    if (!alcCloseDevice(p_alcDevice)) {
        std::cerr << "Failed to close sound device \"" << name_ << "\"\n";
    }
    isOpen = false;
    debug_log("Leaving AudioDevice destructor");
}
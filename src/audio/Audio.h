#ifndef AUDIO_H
#define AUDIO_H

#include "AudioSettings.h"

#include <string>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

#include <sndfile/sndfile.h>
#include "OpenALError.h"


class Audio {

public:

    Audio();
    Audio(const std::string filepath);
    ~Audio();

    operator ALuint() const { return _bufferID; }

private:

    ALuint _bufferID;
    std::string _filepath;

};

#endif // AUDIO_H

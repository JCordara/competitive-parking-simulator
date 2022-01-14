#ifndef AUDIO_H
#define AUDIO_H

#include <string>

#define AL_LIBTYPE_STATIC

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

    ALuint id() const { return bufferID; }

private:

    ALuint bufferID;

};

#endif // AUDIO_H

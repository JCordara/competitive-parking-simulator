#ifndef AUDIO_H
#define AUDIO_H

#include <string>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#include "AL/efx.h"
#include "OpenALError.h"

#include "sndfile.h"


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

#include "Audio.h"

Audio::Audio() 
    : Audio("audio/null.wav")
{}

Audio::Audio(const std::string filepath) 
    : _bufferID(0)
{

    _filepath = filepath;

    ALenum format;
    SNDFILE* sndfile;
    SF_INFO sfinfo;

    short* membuf;
    sf_count_t num_frames;

    memset(&sfinfo, 0, sizeof (sfinfo));

    // Open the audio file and check that it's usable
    if (!(sndfile = sf_open(filepath.c_str(), SFM_READ, &sfinfo))) {
        std::cerr << "Could not open audio in " << filepath << ":\n";
        std::cerr << sf_strerror(sndfile) << "\n";
        return;
    }

    // Check that the file is an allowable size
    sf_count_t max_frames;
    max_frames = (sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels;

    if (sfinfo.frames < 1 || sfinfo.frames > max_frames) {
        std::cerr << "Bad sample count in " << filepath 
                    << " (" << sfinfo.frames << ")\n";
        sf_close(sndfile);
        return;
    }

    // Get the sound format, and figure out the OpenAL format
    format = AL_NONE;
    
    if ((sfinfo.format & SF_FORMAT_PCM_U8) == SF_FORMAT_PCM_U8
     || (sfinfo.format & SF_FORMAT_PCM_S8) == SF_FORMAT_PCM_S8) {
        // If 8 bits per sample
        if (sfinfo.channels == 1)
            format = AL_FORMAT_MONO8;
        else if (sfinfo.channels == 2)
            format = AL_FORMAT_STEREO8;
    
    } else {
        // If 16 or more bits per sample
        if (sfinfo.channels == 1)
            format = AL_FORMAT_MONO16;
        else if (sfinfo.channels == 2)
            format = AL_FORMAT_STEREO16;
    }
    
    if(!format) {
        std::cerr << "Unsupported channel count: " 
                    << sfinfo.channels << "\n";
        sf_close(sndfile);
        return;
    }

    // Decode the whole audio file to a buffer
    membuf = static_cast<short*>(
        malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)) 
    );

    // Allow reading of float encoded wav files as 16 bit integer values
    sf_command(sndfile, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);
    
    // Read audio file into membuf buffer
    num_frames = sf_read_short(sndfile, membuf, sfinfo.frames * sfinfo.channels);

    // Check that it was read correctly
    if(num_frames != sfinfo.frames * sfinfo.channels) {
        free(membuf);
        sf_close(sndfile);
        std::cerr << "Failed to read samples in " << filepath 
                    << " ("  << num_frames << ")\n";
        exit(1);
    }

    // Determine the number of bytes in the buffer
    ALsizei num_bytes = (ALsizei)(num_frames) *
                        (ALsizei)sizeof(short);

#if 0
    for (int i = 0; i < num_bytes; i++) {
        std::cout << membuf[i] << " ";
    }
#endif


    // Copy the audio data into a new buffer object
    alGenBuffers(1, &_bufferID);
    alBufferData(_bufferID, format, membuf, num_bytes, sfinfo.samplerate);

    // Release resources
    free(membuf);
    sf_close(sndfile);

    // Check if an error occured, and clean up if so
    ALenum err = alGetError();
    if(err != AL_NO_ERROR) {
        std::cerr << "OpenAL Error: " << alGetString(err) << "\n";
        if (_bufferID && alIsBuffer(_bufferID))
            alDeleteBuffers(1, &_bufferID);
        return;
    }
}

Audio::~Audio() {
    debug_log("Entering Audio destructor");
    if(alIsBuffer(_bufferID)) alDeleteBuffers(1, &_bufferID);
    debug_log("Leaving Audio destructor");
}
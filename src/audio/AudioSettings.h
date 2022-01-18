/**
 * Contains global audio settings
 */

#define DOPPLER_SCALE_FACTOR 0.3    // Scale up/down doppler effect

#ifdef DEBUG
    #define debug_log(s)\
    std::cerr << "> " << s << "\n";
#else
    #define debug_log(s)
#endif
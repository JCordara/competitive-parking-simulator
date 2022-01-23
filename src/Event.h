/**
 * @file Event.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef EVENT_H
#define EVENT_H

#include "EventTypes.h" // Defines event framework

// Dependencies are linked through this class
// Event.h knows all
#include "AudioSystem.h"

namespace Events {
    extern Event<void> GameStart;
    extern Event<void> TestAudio;
    extern Event<Audio&> TestAudioParameter;
};

#endif // EVENT_H

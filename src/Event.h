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
#include "AudioManager.h"

namespace Events {
    extern Event<void> GameStart;
    extern Event<void> TestAudio;
    extern Event<Audio&> TestAudioParameter;

    extern Event<void> PressW;
    extern Event<void> PressA;
    extern Event<void> PressS;
    extern Event<void> PressD;
    extern Event<void> ReleaseW;
    extern Event<void> ReleaseA;
    extern Event<void> ReleaseS;
    extern Event<void> ReleaseD;
    extern Event<void> PressSpace;
    extern Event<void> ReleaseSpace;
	extern Event<void> ReleaseLeftShift;
	extern Event<void> PressLeftShift;
};

#endif // EVENT_H

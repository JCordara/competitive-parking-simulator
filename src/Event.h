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


namespace Events {
    extern Event<void> GameStart;
    extern Event<void> CarParked;
    extern Event<void> CarUnParked;

    extern Event<void> PlayerAccelerate;
    extern Event<void> PlayerAccelerateRelease;
    extern Event<void> PlayerReverse;
    extern Event<void> PlayerReverseRelease;
    extern Event<void> PlayerTurnLeft;
    extern Event<void> PlayerTurnLeftRelease;
    extern Event<void> PlayerTurnRight;
    extern Event<void> PlayerTurnRightRelease;
    extern Event<void> PlayerBrake;
    extern Event<void> PlayerBrakeRelease;
	extern Event<void> PlayerHandbrake;
	extern Event<void> PlayerHandbrakeRelease;

    extern Event<double, double> CameraRotate;
    extern Event<double, double> CameraZoom;
};

#endif // EVENT_H

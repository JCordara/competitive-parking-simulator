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

// Forward declaration of Event parameter types
class RigidbodyComponent;

namespace Events {
    extern Event<void> GameStart;
    extern Event<void> CarParked;
    extern Event<void> CarUnParked;

    extern Event<float> PlayerAccelerate;
    extern Event<float> PlayerSteer;
    extern Event<float> PlayerBrake;
	extern Event<float> PlayerHandbrake;

    extern Event<double, double> CameraRotate;
    extern Event<double, double> CameraZoom;

    // Broadcast by rigidbody components when they are initalized
    extern Event<RigidbodyComponent&> RigidbodyComponentInit;
    // Broadcast by rigidbody components when they are initalized
    // extern Event<VehicleComponent&> VehicleComponentInit;
};

#endif // EVENT_H

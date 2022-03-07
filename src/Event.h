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
class Entity;
class RigidbodyComponent;
class VehicleComponent;
class ControllerComponent;

namespace Events {
    extern Event<void> GameStart;
    extern Event<void> CarParked;
    extern Event<void> CarUnParked;

    extern Event<Entity&, float> VehicleAccelerate;
    extern Event<Entity&, float> VehicleSteer;
    extern Event<Entity&, float> VehicleBrake;
	extern Event<Entity&, float> VehicleHandbrake;

    extern Event<double, double> CameraRotate;
    extern Event<double, double> CameraZoom;

    extern Event<void> CarBoxCollision;

    // Broadcast by rigidbody components when they are initalized
    extern Event<RigidbodyComponent&> RigidbodyComponentInit;
    // Broadcast by vehicle components when they are initalized
    extern Event<VehicleComponent&> VehicleComponentInit;
    // Broadcast by controller components when they are initalized
    extern Event<ControllerComponent&> ControllerComponentInit;
};

#endif // EVENT_H

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

#include "Components/Entity.h"

// Forward declaration of Event parameter types
class RigidbodyComponent;
class VehicleComponent;
class ControllerComponent;
class AiComponent;
class AudioComponent;
class VolumeTriggerComponent;

namespace Events {
    extern Event<void> GameStart;
	extern Event<void> GamePlay;
	extern Event<void> GameExit;
	extern Event<void> GameOptions;
	extern Event<void> GameWon;
	extern Event<shared_ptr<Entity>, float> GameReset;
    extern Event<shared_ptr<Entity>> CarParked;
    extern Event<void> CarUnParked;

    extern Event<void> RecompileScripts;

    extern Event<shared_ptr<Entity>, float> VehicleAccelerate;
    extern Event<shared_ptr<Entity>, float> VehicleSteer;
    extern Event<shared_ptr<Entity>, float> VehicleBrake;
	extern Event<shared_ptr<Entity>, float> VehicleHandbrake;
	extern Event<shared_ptr<Entity>, float> VehicleFlip;

    extern Event<double, double> CameraRotate;
    extern Event<double, double> CameraZoom;

    extern Event<int, int> WindowResized;

    extern Event<shared_ptr<Entity>, shared_ptr<Entity>> Collision;

    // Broadcast by rigidbody components when they are initalized
    extern Event<RigidbodyComponent&> RigidbodyComponentInit;
    // Broadcast by vehicle components when they are initalized
    extern Event<VehicleComponent&> VehicleComponentInit;
    // Broadcast by controller components when they are initalized
    extern Event<ControllerComponent&> ControllerComponentInit;
    // Broadcast by ai components when they are initalized
    extern Event<AiComponent&> AiComponentInit;
    // Broadcast by audio components when they are initalized
    extern Event<AudioComponent&> AudioComponentInit;
	// Broadcast by volumetrigger components when they are initalized
	extern Event<VolumeTriggerComponent&> VolumeTriggerComponentInit;

    extern Event<bool> TestUiEvent;
    extern Event<float> ChangeMusicVolume;
    extern Event<int> ChangeNumAI;

};

#endif // EVENT_H

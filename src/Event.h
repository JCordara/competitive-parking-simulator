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
#include "Common.h"

// Forward declaration of Event parameter types
class RigidbodyComponent;
class VehicleComponent;
class ControllerComponent;
class AiComponent;
class AudioComponent;
class VolumeTriggerComponent;

namespace Events {
	/* --- Application Framework Events --- */
	extern Event<void> ExitApplication;
	extern Event<int, int> WindowResized;
	/* --- GUI Swap Events --- */
	extern Event<void> MainMenu;
	extern Event<void> GameGUI;
	extern Event<void> RoundEndGUI;
	extern Event<string> GameEndGUI;
	/* --- Entity Manipulation Events--- */
	extern Event<string, instancedTransformation> AddPropCar;
	extern Event<string, instancedTransformation> AddParkingSpace;
	extern Event<string> AddAICar;
	/* --- Player Controller Events --- */
	extern Event<shared_ptr<Entity>, float> VehicleAccelerate;
	extern Event<shared_ptr<Entity>, float> VehicleSteer;
	extern Event<shared_ptr<Entity>, float> VehicleBrake;
	extern Event<shared_ptr<Entity>, float> VehicleHandbrake;
	extern Event<shared_ptr<Entity>, float> VehicleFlip;
	/* --- Physx --- */
	extern Event<shared_ptr<Entity>, shared_ptr<Entity>> Collision;
	/* --- Components --- */
	extern Event<RigidbodyComponent&> RigidbodyComponentInit;
	extern Event<VehicleComponent&> VehicleComponentInit;
	extern Event<ControllerComponent&> ControllerComponentInit;
	extern Event<AiComponent&> AiComponentInit;
	extern Event<AudioComponent&> AudioComponentInit;
	extern Event<VolumeTriggerComponent&> VolumeTriggerComponentInit;
	/* --- Game Logic Events --- */
	extern Event<void> NewGame;
	extern Event<void> NextRound;
	extern Event<void> EndGame;
	extern Event<shared_ptr<Entity>, float> GameReset;
	extern Event<shared_ptr<Entity>> CarParked;
	extern Event<void> CarUnParked;
	/* --- Script Events --- */
	extern Event<void> RecompileScripts;
	/* --- miscellaneous Events --- */
    extern Event<float> ChangeMusicVolume;
};

#endif // EVENT_H

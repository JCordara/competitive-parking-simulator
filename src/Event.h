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
	extern Event<void> GameOptions;
	/* --- Entity Manipulation Events--- */
	extern Event<string, instancedTransformation> AddPropCar;
	extern Event<string, instancedTransformation> AddParkingSpace;
	extern Event<string> AddAICar;
	/* --- Player Controller Events --- */
	extern Event<weak_ptr<Entity>, float> VehicleAccelerate;
	extern Event<weak_ptr<Entity>, float> VehicleSteer;
	extern Event<weak_ptr<Entity>, float> VehicleBrake;
	extern Event<weak_ptr<Entity>, float> VehicleHandbrake;
	extern Event<weak_ptr<Entity>, float> VehicleFlip;
	/* --- Physx --- */
	extern Event<weak_ptr<Entity>, weak_ptr<Entity>> Collision;
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
	extern Event<weak_ptr<Entity>, float> GameReset;
	extern Event<weak_ptr<Entity>, weak_ptr<Entity>> CarParked;
	extern Event<void> CarUnParked;
	/* --- Script Events --- */
	extern Event<void> RecompileScripts;
	/* --- miscellaneous Events --- */
    extern Event<float> ChangeMusicVolume;
	extern Event<int> ChangeNumberOfAI;
	extern Event<int> Fullscreen;
};

#endif // EVENT_H

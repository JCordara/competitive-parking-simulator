#include "Event.h"

/* --- Application Framework Events --- */
Event<void> ExitApplication;
Event<int, int> WindowResized;
/* --- GUI Swap Events --- */
Event<void> MainMenu;
Event<void> GameGUI;
Event<void> RoundEndGUI;
Event<string> GameEndGUI;
/* --- Entity Manipulation Events--- */
Event<string, instancedTransformation> AddPropCar;
Event<string, instancedTransformation> AddParkingSpace;
Event<string, instancedTransformation> AddAICar;
/* --- Player Controller Events --- */
Event<shared_ptr<Entity>, float> VehicleAccelerate;
Event<shared_ptr<Entity>, float> VehicleSteer;
Event<shared_ptr<Entity>, float> VehicleBrake;
Event<shared_ptr<Entity>, float> VehicleHandbrake;
Event<shared_ptr<Entity>, float> VehicleFlip;
/* --- Physx --- */
Event<shared_ptr<Entity>, shared_ptr<Entity>> Collision;
/* --- Components --- */
Event<RigidbodyComponent&> RigidbodyComponentInit;
Event<VehicleComponent&> VehicleComponentInit;
Event<ControllerComponent&> ControllerComponentInit;
Event<AiComponent&> AiComponentInit;
Event<AudioComponent&> AudioComponentInit;
Event<VolumeTriggerComponent&> VolumeTriggerComponentInit;
/* --- Game Logic Events --- */
Event<void> GameStart;
Event<void> GamePlay;
Event<void> GameOptions;
Event<void> GameWon;
Event<shared_ptr<Entity>, float> GameReset;
Event<shared_ptr<Entity>> CarParked;
Event<void> CarUnParked;
/* --- Script Events --- */
Event<void> RecompileScripts;
/* --- Miscellaneous Events --- */
Event<float> ChangeMusicVolume;




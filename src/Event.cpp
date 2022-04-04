#include "Event.h"

/* --- Application Framework Events --- */
Event<void> Events::ExitApplication;
Event<int, int> Events::WindowResized;
/* --- GUI Swap Events --- */
Event<void> Events::MainMenu;
Event<void> Events::GameGUI;
Event<void> Events::RoundEndGUI;
Event<string> Events::GameEndGUI;
/* --- Entity Manipulation Events--- */
Event<string, instancedTransformation> Events::AddPropCar;
Event<string, instancedTransformation> Events::AddParkingSpace;
Event<string> Events::AddAICar;
/* --- Player Controller Events --- */
Event<shared_ptr<Entity>, float> Events::VehicleAccelerate;
Event<shared_ptr<Entity>, float> Events::VehicleSteer;
Event<shared_ptr<Entity>, float> Events::VehicleBrake;
Event<shared_ptr<Entity>, float> Events::VehicleHandbrake;
Event<shared_ptr<Entity>, float> Events::VehicleFlip;
/* --- Physx --- */
Event<shared_ptr<Entity>, shared_ptr<Entity>> Events::Collision;
/* --- Components --- */
Event<RigidbodyComponent&> Events::RigidbodyComponentInit;
Event<VehicleComponent&> Events::VehicleComponentInit;
Event<ControllerComponent&> Events::ControllerComponentInit;
Event<AiComponent&> Events::AiComponentInit;
Event<AudioComponent&> Events::AudioComponentInit;
Event<VolumeTriggerComponent&> Events::VolumeTriggerComponentInit;
/* --- Game Logic Events --- */
Event<void> Events::NewGame;
Event<void> Events::NextRound;
Event<void> Events::EndGame;
Event<shared_ptr<Entity>, float> Events::GameReset;
Event<shared_ptr<Entity>, shared_ptr<Entity>> Events::CarParked;
Event<void> Events::CarUnParked;
/* --- Script Events --- */
Event<void> Events::RecompileScripts;
/* --- Miscellaneous Events --- */
Event<float> Events::ChangeMusicVolume;




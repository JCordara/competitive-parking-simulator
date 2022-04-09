#include "Event.h"

/* --- Application Framework Events --- */
Event<void> Events::ExitApplication;
Event<int, int> Events::WindowResized;
/* --- GUI Swap Events --- */
Event<void> Events::MainMenu;
Event<void> Events::GameGUI;
Event<void> Events::RoundEndGUI;
Event<string> Events::GameEndGUI;
Event<void> Events::GameOptions;
/* --- Entity Manipulation Events--- */
Event<string, instancedTransformation> Events::AddPropCar;
Event<string, instancedTransformation> Events::AddParkingSpace;
Event<string> Events::AddAICar;
/* --- Player Controller Events --- */
Event<weak_ptr<Entity>, float> Events::VehicleAccelerate;
Event<weak_ptr<Entity>, float> Events::VehicleSteer;
Event<weak_ptr<Entity>, float> Events::VehicleBrake;
Event<weak_ptr<Entity>, float> Events::VehicleHandbrake;
Event<weak_ptr<Entity>, float> Events::VehicleFlip;
/* --- Physx --- */
Event<weak_ptr<Entity>, weak_ptr<Entity>> Events::Collision;
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
Event<weak_ptr<Entity>, float> Events::GameReset;
Event<weak_ptr<Entity>, weak_ptr<Entity>> Events::CarParked;
Event<void> Events::CarUnParked;
/* --- Script Events --- */
Event<void> Events::RecompileScripts;
/* --- Miscellaneous Events --- */
Event<float> Events::ChangeMusicVolume;
Event<int>  Events::ChangeNumberOfAI;
Event<int> Events::Fullscreen;




#include "Event.h"

Event<void> Events::GameStart;
Event<void> Events::GameWon;
Event<shared_ptr<Entity>, float> Events::GameReset;
Event<shared_ptr<Entity>> Events::CarParked;
Event<void> Events::CarUnParked;

Event<void> Events::RecompileScripts;


Event<shared_ptr<Entity>, float> Events::VehicleAccelerate;
Event<shared_ptr<Entity>, float> Events::VehicleSteer;
Event<shared_ptr<Entity>, float> Events::VehicleBrake;
Event<shared_ptr<Entity>, float> Events::VehicleHandbrake;
Event<shared_ptr<Entity>, float> Events::VehicleFlip;

Event<double, double> Events::CameraRotate;
Event<double, double> Events::CameraZoom;

Event<glm::vec3&> Events::Collision;


Event<RigidbodyComponent&> Events::RigidbodyComponentInit;
Event<VolumeTriggerComponent&> Events::VolumeTriggerComponentInit;
Event<VehicleComponent&> Events::VehicleComponentInit;
Event<ControllerComponent&> Events::ControllerComponentInit;
Event<AiComponent&> Events::AiComponentInit;
Event<AudioComponent&> Events::AudioComponentInit;



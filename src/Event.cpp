#include "Event.h"

Event<void> Events::GameStart;
Event<void> Events::GameWon;
Event<Entity&, float> Events::GameReset;
Event<Entity&> Events::CarParked;
Event<void> Events::CarUnParked;

Event<Entity&, float> Events::VehicleAccelerate;
Event<Entity&, float> Events::VehicleSteer;
Event<Entity&, float> Events::VehicleBrake;
Event<Entity&, float> Events::VehicleHandbrake;

Event<double, double> Events::CameraRotate;
Event<double, double> Events::CameraZoom;

Event<glm::vec3&> Events::Collision;


Event<RigidbodyComponent&> Events::RigidbodyComponentInit;
Event<VolumeTriggerComponent&> Events::VolumeTriggerComponentInit;
Event<VehicleComponent&> Events::VehicleComponentInit;
Event<ControllerComponent&> Events::ControllerComponentInit;
Event<AiComponent&> Events::AiComponentInit;
Event<AudioComponent&> Events::AudioComponentInit;



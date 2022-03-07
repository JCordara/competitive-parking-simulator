#include "Event.h"

Event<void> Events::GameStart;
Event<void> Events::CarParked;
Event<void> Events::CarUnParked;

Event<Entity&, float> Events::VehicleAccelerate;
Event<Entity&, float> Events::VehicleSteer;
Event<Entity&, float> Events::VehicleBrake;
Event<Entity&, float> Events::VehicleHandbrake;

Event<double, double> Events::CameraRotate;
Event<double, double> Events::CameraZoom;

Event<void> Events::CarBoxCollision;


Event<RigidbodyComponent&> Events::RigidbodyComponentInit;
Event<VehicleComponent&> Events::VehicleComponentInit;
Event<ControllerComponent&> Events::ControllerComponentInit;
Event<AiComponent&> Events::AiComponentInit;
Event<AudioComponent&> Events::AudioComponentInit;



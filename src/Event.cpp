#include "Event.h"

Event<void> Events::GameStart;
Event<void> Events::CarParked;
Event<void> Events::CarUnParked;

Event<float> Events::PlayerAccelerate;
Event<float> Events::PlayerSteer;
Event<float> Events::PlayerBrake;
Event<float> Events::PlayerHandbrake;

Event<double, double> Events::CameraRotate;
Event<double, double> Events::CameraZoom;


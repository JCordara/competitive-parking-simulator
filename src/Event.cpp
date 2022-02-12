#include "Event.h"

Event<void> Events::GameStart;
Event<void> Events::CarParked;
Event<void> Events::CarUnParked;

Event<void> Events::PlayerAccelerate;
Event<void> Events::PlayerAccelerateRelease;
Event<void> Events::PlayerReverse;
Event<void> Events::PlayerReverseRelease;
Event<void> Events::PlayerTurnLeft;
Event<void> Events::PlayerTurnLeftRelease;
Event<void> Events::PlayerTurnRight;
Event<void> Events::PlayerTurnRightRelease;
Event<void> Events::PlayerBrake;
Event<void> Events::PlayerBrakeRelease;
Event<void> Events::PlayerHandbrake;
Event<void> Events::PlayerHandbrakeRelease;

Event<double, double> Events::CameraRotate;
Event<double, double> Events::CameraZoom;


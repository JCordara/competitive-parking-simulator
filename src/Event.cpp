#include "Event.h"

Event<void> Events::GameStart;
Event<void> Events::TestAudio;
Event<Audio&> Events::TestAudioParameter;

Event<void> Events::MoveForward;
Event<void> Events::MoveBackward;
Event<void> Events::MoveLeft;
Event<void> Events::MoveRight;
Event<void> Events::Brake;
Event<void> Events::ReleaseDriveControls;
Event<void> Events::ReleaseTurnControls;

#include "Event.h"

Event<void> Events::GameStart;
Event<void> Events::TestAudio;
Event<Audio&> Events::TestAudioParameter;

Event<void> Events::PressW;
Event<void> Events::PressA;
Event<void> Events::PressS;
Event<void> Events::PressD;
Event<void> Events::ReleaseW;
Event<void> Events::ReleaseA;
Event<void> Events::ReleaseS;
Event<void> Events::ReleaseD;
Event<void> Events::PressSpace;
Event<void> Events::ReleaseSpace;

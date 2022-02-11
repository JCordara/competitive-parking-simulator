#ifndef AUDIO_COMPONENT_H
#define AUDIO_COMPONENT_H

#include "Components.h"


class AudioComponent : public BaseComponent {
public:
    AudioComponent(Entity& parent);
    ~AudioComponent();
    static ComponentEnum getType();
};

#endif // AUDIO_COMPONENT_H

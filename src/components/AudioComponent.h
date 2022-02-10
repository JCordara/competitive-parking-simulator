#ifndef AUDIO_COMPONENT_H
#define AUDIO_COMPONENT_H

#include "Components.h"


class AudioComponent : public BaseComponent {
public:
    static ComponentEnum getType();
    ~AudioComponent();
};

#endif // AUDIO_COMPONENT_H

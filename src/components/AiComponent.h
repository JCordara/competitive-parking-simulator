#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H

#include "Components.h"


class AiComponent : public BaseComponent {
public:
    static  ComponentEnum getType();
    ~AiComponent();
};

#endif // AI_COMPONENT_H

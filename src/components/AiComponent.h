#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H

#include "Components.h"


class AiComponent : public BaseComponent {
public:
    AiComponent(Entity& parent);
    ~AiComponent();
    static  ComponentEnum getType();
};

#endif // AI_COMPONENT_H

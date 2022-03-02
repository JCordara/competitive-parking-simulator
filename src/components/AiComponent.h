#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H

#include "Components.h"


class AiComponent : public BaseComponent {
public:
    AiComponent(Entity& parent);
    ~AiComponent();
    static ComponentEnum getType();

    void update();

private:

    std::vector<Entity> carQueue;
    // std::vector<Something> nodeQueue;

    float sightFrontConeWidth;
    float sightFrontConeHeight;
    float sightBackConeWidth;
    float sightBackConeHeight;

};

#endif // AI_COMPONENT_H

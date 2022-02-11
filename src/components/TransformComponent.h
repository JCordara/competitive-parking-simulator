#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "Components.h"


class TransformComponent : public BaseComponent {
public:
    
    TransformComponent(Entity& parent);

    static  ComponentEnum getType();
    ~TransformComponent();


    // Test attributes
    float x, y, z;
};

#endif // TRANSFORM_COMPONENT_H
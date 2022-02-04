#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "BaseComponent.h"

/**
 * @brief Example component that implements the BaseComponent interface.
 */
class TransformComponent : public BaseComponent {
public:
    
    TransformComponent();

    static  ComponentEnum getType();
    ~TransformComponent();


    // Test attributes
    float x, y, z;

    TransformComponent(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }
};

#endif // TRANSFORM_COMPONENT_H
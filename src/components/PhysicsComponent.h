#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "BaseComponent.h"

class PhysicsComponent : public BaseComponent {
public:
    static ComponentEnum getType();
    ~PhysicsComponent();
};

#endif // PHYSICS_COMPONENT_H

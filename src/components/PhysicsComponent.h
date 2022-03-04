#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Components.h"



class PhysicsComponent : public BaseComponent {
public:
    PhysicsComponent(Entity& parent);
    ~PhysicsComponent();
    static ComponentEnum getType();
	void addActor();
};

#endif // PHYSICS_COMPONENT_H

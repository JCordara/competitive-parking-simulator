#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Components.h"
#include <PhysX/PxPhysicsAPI.h>

using namespace physx;

class PhysicsComponent : public BaseComponent {
public:
    PhysicsComponent(Entity& parent);
    ~PhysicsComponent();
    static ComponentEnum getType();
	void addActor(std::vector<PxVec3> convexVerts);
};

#endif // PHYSICS_COMPONENT_H

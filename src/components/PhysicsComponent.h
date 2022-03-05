#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Components.h"
#include <PhysX/PxPhysicsAPI.h>

class PhysicsSystem;

using namespace physx;

class PhysicsComponent : public BaseComponent {
public:

    PhysicsComponent(Entity& parent);
    ~PhysicsComponent();
    static ComponentEnum getType();
	void addActor(std::vector<PxVec3> convexVerts);

    void setPhysicsSystem(shared_ptr<PhysicsSystem> physics);

private:
    shared_ptr<PhysicsSystem> physicsSystem;
};

#endif // PHYSICS_COMPONENT_H

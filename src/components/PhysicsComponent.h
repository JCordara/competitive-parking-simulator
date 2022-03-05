#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Components.h"
#include "Model.h"
#include <PhysX/PxPhysicsAPI.h>

class PhysicsSystem;

using namespace physx;

class PhysicsComponent : public BaseComponent {
public:

    PhysicsComponent(Entity& parent);
    ~PhysicsComponent();
    static ComponentEnum getType();
	void addActorStatic(Model model, PxTransform startPos);
	void addActorDynamic(Model model, PxTransform startPos);

    void setPhysicsSystem(shared_ptr<PhysicsSystem> physics);

private:
    shared_ptr<PhysicsSystem> physicsSystem;
};

#endif // PHYSICS_COMPONENT_H

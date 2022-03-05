#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Components.h"
#include "Model.h"
#include <PhysX/PxPhysicsAPI.h>

class PhysicsSystem;

using namespace physx;

class RigidbodyComponent : public BaseComponent {
public:

    RigidbodyComponent(Entity& parent);
    ~RigidbodyComponent();
    static ComponentEnum getType();
    
	void addActorStatic(Model model, PxTransform startPos);
	void addActorDynamic(Model model, PxTransform startPos);

    inline void setPhysicsSystem(shared_ptr<PhysicsSystem> physics) { 
        physicsSystem = physics; 
    }

private:
    shared_ptr<PhysicsSystem> physicsSystem;
};

#endif // PHYSICS_COMPONENT_H

#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Components.h"
#include "Model.h"
#include <PhysX/PxPhysicsAPI.h>

class PhysicsSystem;

using namespace physx;

class RigidbodyComponent : public BaseComponent {
public:

    RigidbodyComponent(shared_ptr<Entity> parent);
    ~RigidbodyComponent();
    static ComponentEnum getType();
    
	void addActorStaticMesh(const Model& model, PxTransform startPos);
	void addActorStaticSphere(const float radius, PxTransform startPos);
	void addActorDynamic(const Model& model, PxTransform startPos);

    inline void setPhysicsSystem(shared_ptr<PhysicsSystem> system) {
        physicsSystem = system;
    }

private:

    shared_ptr<PhysicsSystem> physicsSystem;
};

#endif // PHYSICS_COMPONENT_H

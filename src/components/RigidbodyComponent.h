#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Components.h"
#include "Model.h"
#include <PhysX/PxPhysicsAPI.h>

class PhysicsSystem;

using namespace physx;

class RigidbodyComponent : public BaseComponent {
public:

    RigidbodyComponent(weak_ptr<Entity> parent);
    ~RigidbodyComponent();
    static ComponentEnum getType();
    
	void addActorStaticMesh(const Model& model, PxTransform startPos, float sFriction, float dFriction);
	void addActorStaticBox(PxVec3 halfLen, PxTransform startPos);
	void addActorStaticCapsule(const float halfhieght, const float radius, PxTransform startPos, float sFriction, float dFriction);
	void addActorDynamic(const Model& model, PxTransform startPos);

    PxRigidActor* getActor() { return actor; }

    inline void setPhysicsSystem(shared_ptr<PhysicsSystem> system) {
        physicsSystem = system;
    }

private:
    shared_ptr<PhysicsSystem> physicsSystem;
    PxRigidActor* actor;
};

#endif // PHYSICS_COMPONENT_H

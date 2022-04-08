#ifndef VOLUME_TRIGGER_COMPONENT_H
#define VOLUME_TRIGGER_COMPONENT_H

#include "Components.h"
#include "Model.h"

class PhysicsSystem;
using namespace physx;
class VolumeTriggerComponent : public BaseComponent {
public:

    VolumeTriggerComponent(weak_ptr<Entity> e);

	// void attachEntity(weak_ptr<Entity> collisionEntity);
	// void removeEntity(weak_ptr<Entity> collisionEntity);
	// void flush();
	void createVolumeShape(PxTransform startPos, PxBoxGeometry boxGeom);
    static  ComponentEnum getType();
    ~VolumeTriggerComponent();
	inline void setPhysicsSystem(shared_ptr<PhysicsSystem> system) {
		physicsSystem = system;
	}

private:
	// std::unordered_set<weak_ptr<Entity>> attached;
	shared_ptr<PhysicsSystem> physicsSystem;
	PxRigidActor *actor;
};

#endif // VOLUME_TRIGGER_COMPONENT_H

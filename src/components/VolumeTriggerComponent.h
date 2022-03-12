#ifndef VOLUME_TRIGGER_COMPONENT_H
#define VOLUME_TRIGGER_COMPONENT_H

#include "Components.h"
#include <set>
#include "Model.h"
#include <PhysX/PxPhysicsAPI.h>

class VolumeTriggerComponent : public BaseComponent {
public:

    VolumeTriggerComponent(shared_ptr<Entity> e);

	void attachEntity(std::shared_ptr<Entity> collisionEntity);
	void removeEntity(std::shared_ptr<Entity> collisionEntity);
	void flush();
	void createVolumeShape(PxTransform startPos, PxBoxGeometry boxGeom);
    static  ComponentEnum getType();
    ~VolumeTriggerComponent();
	inline void setPhysicsSystem(shared_ptr<PhysicsSystem> system) {
		physicsSystem = system;
	}

private:
	std::set<std::shared_ptr<Entity>> attached;
	shared_ptr<PhysicsSystem> physicsSystem;
};

#endif // VOLUME_TRIGGER_COMPONENT_H

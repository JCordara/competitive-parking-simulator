#include "VolumeTriggerComponent.h"
#include "physics/PhysicsSystem.h"

VolumeTriggerComponent::VolumeTriggerComponent(Entity& e)
	: BaseComponent(e)
	
{
	Events::VolumeTriggerComponentInit.broadcast(*this);
}

void VolumeTriggerComponent::attachEntity(std::shared_ptr<Entity> collisionEntity) {
	auto it = attached.find(collisionEntity);
	if (it != attached.end())
		attached.insert(collisionEntity);
}
void VolumeTriggerComponent::removeEntity(std::shared_ptr<Entity> collisionEntity) {
	auto it = attached.find(collisionEntity);
	if (it != attached.end())
		attached.erase(it);
}
void VolumeTriggerComponent::flush() {
	attached.clear();
}

void VolumeTriggerComponent::createVolumeShape(PxTransform startPos, PxBoxGeometry boxGeom) {
	physicsSystem->createTriggerBox(startPos, boxGeom);
}

ComponentEnum VolumeTriggerComponent::getType() {
    return ComponentEnum::volumeTrigger;
}

VolumeTriggerComponent::~VolumeTriggerComponent() {
    // Nothing to do here yet
}

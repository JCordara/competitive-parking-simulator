#include "VolumeTriggerComponent.h"
#include "Physics/PhysicsSystem.h"

VolumeTriggerComponent::VolumeTriggerComponent(shared_ptr<Entity> e)
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
	actor = physicsSystem->createTriggerBox(startPos, boxGeom);
}

ComponentEnum VolumeTriggerComponent::getType() {
    return ComponentEnum::volumeTrigger;
}

VolumeTriggerComponent::~VolumeTriggerComponent() {
	 physicsSystem->pxScene->removeActor(*actor, true);
    // Nothing to do here yet
}

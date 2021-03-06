#include "VolumeTriggerComponent.h"
#include "Physics/PhysicsSystem.h"

VolumeTriggerComponent::VolumeTriggerComponent(weak_ptr<Entity> e)
	: BaseComponent(e)
	
{
	Events::VolumeTriggerComponentInit.broadcast(*this);
}

// void VolumeTriggerComponent::attachEntity(std::weak_ptr<Entity> collisionEntity) {
// 	auto it = attached.find(collisionEntity);
// 	if (it != attached.end())
// 		attached.insert(collisionEntity);
// }
// void VolumeTriggerComponent::removeEntity(std::weak_ptr<Entity> collisionEntity) {
// 	auto it = attached.find(collisionEntity);
// 	if (it != attached.end())
// 		attached.erase(it);
// }
// void VolumeTriggerComponent::flush() {
// 	attached.clear();
// }

void VolumeTriggerComponent::createVolumeShape(PxTransform startPos, PxBoxGeometry boxGeom) {
	actor = physicsSystem->createTriggerBox(startPos, boxGeom);
	auto e = getEntity(); if (!e) return;
	actor->userData = static_cast<void*>(e.get());
}

ComponentEnum VolumeTriggerComponent::getType() {
    return ComponentEnum::volumeTrigger;
}

VolumeTriggerComponent::~VolumeTriggerComponent() {
	// Remove triggerbox from physics scene
	physicsSystem->pxScene->removeActor(*actor, true);
}

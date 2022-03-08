#include "VolumeTriggerComponent.h"


VolumeTriggerComponent::VolumeTriggerComponent(Entity& e) 
    : BaseComponent(e)
{}

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

void VolumeTriggerComponent::createVolumeShape(PxActor& actor) {
	/*PxShape* treasureShape;
	actor->getShapes(&treasureShape, 1);
	treasureShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	treasureShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);*/
}

ComponentEnum VolumeTriggerComponent::getType() {
    return ComponentEnum::volumeTrigger;
}

VolumeTriggerComponent::~VolumeTriggerComponent() {
    // Nothing to do here yet
}

#include "VolumeTriggerComponent.h"


VolumeTriggerComponent::VolumeTriggerComponent(Entity& e) 
    : BaseComponent(e)
    , occupied(false)
{}

void VolumeTriggerComponent::checkForEntity(GameObject& object) {
    glm::mat4 m = object.getTransformation();
    glm::vec3 entityPos = m[3];
    
    auto transform = entity.getComponent<TransformComponent>();
    if (!transform) return;

    glm::vec3 selfPos = transform->getGlobalPosition();

    if (!occupied) {
        if (glm::distance(entityPos, selfPos) < 2.0f) {
            occupied = true;
            Events::CarParked.broadcast();
        }
    }
    else {
        if (glm::distance(entityPos, selfPos) > 2.0f) {
            occupied = false;
            Events::CarUnParked.broadcast();
        }
    }
}

ComponentEnum VolumeTriggerComponent::getType() {
    return ComponentEnum::volumeTrigger;
}

VolumeTriggerComponent::~VolumeTriggerComponent() {
    // Nothing to do here yet
}

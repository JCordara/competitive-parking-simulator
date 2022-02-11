#include "VolumeTriggerComponent.h"


VolumeTriggerComponent::VolumeTriggerComponent(Entity& parent) 
    : BaseComponent(parent)
    , occupied(false)
{}

void VolumeTriggerComponent::checkForEntity(GameObject& entity) {
    glm::mat4 m = entity.getTransformation();
    glm::vec3 entityPos = m[3];
    
    auto transform = parent.getComponent<TransformComponent>();
    if (!transform) return;

    glm::vec3 selfPos = glm::vec3(transform->x, transform->y, transform->z);

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

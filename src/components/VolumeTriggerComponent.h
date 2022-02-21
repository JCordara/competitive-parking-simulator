#ifndef VOLUME_TRIGGER_COMPONENT_H
#define VOLUME_TRIGGER_COMPONENT_H

#include "Components.h"

#include "GameObject.h"

class VolumeTriggerComponent : public BaseComponent {
public:

    VolumeTriggerComponent(Entity& e);

    VolumeTriggerComponent(GameObject& gameObject);
    void checkForEntity(GameObject& entity);

    static  ComponentEnum getType();
    ~VolumeTriggerComponent();

private:
    bool occupied;
};

#endif // VOLUME_TRIGGER_COMPONENT_H

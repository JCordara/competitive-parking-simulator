#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Entity.h"
#include "Event.h"


enum class ComponentEnum {
    ai,
    audio,
    controller,
    model,
    physics,        // Is Rigidbody a better name?
    renderer,
    transform,
    volumeTrigger
};

/**
 * @brief Components must be derived from this interface. Components must
 * implement a static getType() class method that returns the appropriate 
 * ComponentEnum value.
 */
class BaseComponent {
public:
    BaseComponent(Entity& p) : parent(p) {}

    virtual ~BaseComponent() {};

protected:
    Entity& parent;
};


#include "AiComponent.h"
#include "AudioComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "VolumeTriggerComponent.h"

#endif // COMPONENTS_H

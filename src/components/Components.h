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
	lighting,
	camera,
    transform,
    volumeTrigger,
	description
};

/**
 * @brief Components must be derived from this interface. Components must
 * implement a static getType() class method that returns the appropriate 
 * ComponentEnum value.
 */
class BaseComponent {
public:
    BaseComponent(Entity& e) : entity(e) {}
    virtual ~BaseComponent() {};
protected:
    Entity& entity;
};


#include "AiComponent.h"
#include "AudioComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "LightingComponent.h"
#include "CameraComponent.h"
#include "VolumeTriggerComponent.h"
#include "DescriptionComponent.h"

#endif // COMPONENTS_H

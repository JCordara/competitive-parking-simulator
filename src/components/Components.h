#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Entity.h"
#include "Event.h"

// Fun fact this is sorted alphabetically and I have OCD
enum class ComponentEnum {
	ai,
	audio,
	controller,
	description,
	model,
	rigidbody,
	renderer,
	lighting,
	camera,
    transform,
	vehicle,
    volumeTrigger
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
#include "DescriptionComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "RigidbodyComponent.h"
#include "RendererComponent.h"
#include "LightingComponent.h"
#include "CameraComponent.h"
#include "VehicleComponent.h"
#include "VolumeTriggerComponent.h"

#endif // COMPONENTS_H

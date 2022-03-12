#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Entity.h"
#include "Event.h"

// Forward declare all component types
class AiComponent;
class AudioComponent;
class ControllerComponent;
class DescriptionComponent;
class ModelComponent;
class TransformComponent;
class RigidbodyComponent;
class RendererComponent;
class LightingComponent;
class CameraComponent;
class VehicleComponent;
class VolumeTriggerComponent;

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
    BaseComponent(shared_ptr<Entity> e) : entity(e) {}
    virtual ~BaseComponent() {};
protected:
    shared_ptr<Entity> entity;
};


#include "AiComponent.h"
#include "AudioComponent.h"
#include "ControllerComponent.h"
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

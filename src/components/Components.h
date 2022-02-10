#ifndef COMPONENTS_H
#define COMPONENTS_H


enum class ComponentEnum {
    ai,
    audio,
    controller,     // Haven't created a controller class yet, needed?
    model,
    physics,        // Is Rigidbody a better name?
    renderer,
    transform
};

/**
 * @brief Components must be derived from this interface. Components must
 * implement a static getType() class method that returns the appropriate 
 * ComponentEnum value.
 */
class BaseComponent {
public:
    virtual ~BaseComponent() = 0;
};


#include "AiComponent.h"
#include "AudioComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"

#endif // COMPONENTS_H

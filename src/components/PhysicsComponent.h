#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Components.h"
#include "Model.h"
#include <PhysX/PxPhysicsAPI.h>

using namespace physx;

class PhysicsComponent : public BaseComponent {
public:
    PhysicsComponent(Entity& parent);
    ~PhysicsComponent();
    static ComponentEnum getType();
	void addActorStatic(Model model, PxTransform startPos);
	void addActorDynamic(Model model, PxTransform startPos);
};

#endif // PHYSICS_COMPONENT_H

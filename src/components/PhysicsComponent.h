#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Components.h"
#include "Model.h"
//using namespace physx;

class PhysicsComponent : public BaseComponent {
public:
    PhysicsComponent(Entity& parent);
    ~PhysicsComponent();
    static ComponentEnum getType();
	void addActorStatic(Model model);
	void addActorDynamic(Model model);
};

#endif // PHYSICS_COMPONENT_H

#include "PhysXCallbacks.h"
#include "Framework.h"


void PhysXSimCallback::onContact(
    const PxContactPairHeader& pairHeader, 
    const PxContactPair* pairs, 
    PxU32 nbPairs) 
{
    for(PxU32 i=0; i < nbPairs; i++) {

        const PxContactPair& cp = pairs[i];

        if(cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            
            Entity* e1 = reinterpret_cast<Entity*>(pairHeader.actors[0]->userData);
            Entity* e2 = reinterpret_cast<Entity*>(pairHeader.actors[1]->userData);

            if (!e1 || !e2) return;

            if ((e1->hasComponent<VehicleComponent>() && e2->hasComponent<RigidbodyComponent>())
            ||  (e1->hasComponent<RigidbodyComponent>() && e2->hasComponent<VehicleComponent>())) {
                Events::CarBoxCollision.broadcast();
            }

        }
    }
}


void PhysXSimCallback::onTrigger(
	PxTriggerPair* pairs,
	PxU32 nbPairs)
{
	
	for (PxU32 i = 0; i < nbPairs; i++)
	{

		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
			PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		Entity* e1 = static_cast<Entity*>(pairs->otherActor->userData);
		if (!e1) return;

		if ((e1->hasComponent<VehicleComponent>() && e1->hasComponent<ControllerComponent>())&& (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)){
			std::cout << nbPairs;
			Events::CarParked.broadcast(*e1);
		}
	}
	
}


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
            
            Entity* e1 = static_cast<Entity*>(pairHeader.actors[0]->userData);
            Entity* e2 = static_cast<Entity*>(pairHeader.actors[1]->userData);

            if (!e1 || !e2) return;

            if ((e1->hasComponent<VehicleComponent>()   && e2->hasComponent<RigidbodyComponent>())
            ||  (e1->hasComponent<RigidbodyComponent>() && e2->hasComponent<VehicleComponent>())
            ||  (e1->hasComponent<VehicleComponent>()   && e2->hasComponent<VehicleComponent>())) {
                glm::vec3 pos = e1->getComponent<TransformComponent>()->getGlobalPosition();
                Events::Collision.broadcast(e1->shared_from_this(), e2->shared_from_this());
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
		//Entity* e0 = static_cast<Entity*>(pairs->triggerActor->userData);
		if (!e1 /* || !e0*/) return;

		if ((e1->hasComponent<VehicleComponent>() && e1->hasComponent<ControllerComponent>())&& (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)){
			// std::cout << nbPairs;
			//if (scene->getEntityByID(playerId)->getComponent<VehicleComponent>()->getSpeed() <= 3.0f) {

			//};
			Events::CarParked.broadcast(e1->shared_from_this());//, e0->shared_from_this());
			pairs->triggerActor->getShapes(e1->getComponent<VehicleComponent>()->triggerShapes, 1, 0);
		}
	}
	
}


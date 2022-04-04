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
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER 
		| PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		// Get raw pointer to car entity
		Entity* pCar = static_cast<Entity*>(pairs->otherActor->userData);
		if (!pCar) return;

		// Get raw pointer to trigger entity
		Entity* pTrigger = static_cast<Entity*>(pairs->triggerActor->userData);
		if (!pTrigger) return;

		// Get shared pointers to entities
		shared_ptr<Entity> car = pCar->shared_from_this();
		shared_ptr<Entity> trigger = pTrigger->shared_from_this();

		// Broadcast when any vehicle collides with a triggerbox
		if ((car->hasComponent<VehicleComponent>()) 
		&& (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)) {
			Events::CarParked.broadcast(car, trigger);
		}

		/*
		// Activated when player collides with triggerbox
		if ((car->hasComponent<VehicleComponent>() && car->hasComponent<ControllerComponent>()) && (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)){
			Events::CarParked.broadcast(car, trigger);

			// std::cout << nbPairs;
			//if (scene->getEntityByID(playerId)->getComponent<VehicleComponent>()->getSpeed() <= 3.0f) {

			//};
<<<<<<< Updated upstream
			Events::CarParked.broadcast(e1->shared_from_this());//, e0->shared_from_this());
			pairs->triggerActor->getShapes(e1->getComponent<VehicleComponent>()->triggerShapes, 1, 0);
=======
			// pairs->triggerActor->getShapes(car->getComponent<VehicleComponent>()->triggerShapes, 1, 0);
		}

		// Activated when an AI collides with triggerbox
		if ((car->hasComponent<VehicleComponent>() && car->hasComponent<AiComponent>()) && (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)) {
			Events::CarParked.broadcast(car, trigger);
>>>>>>> Stashed changes
		}
		*/
	}
	
}


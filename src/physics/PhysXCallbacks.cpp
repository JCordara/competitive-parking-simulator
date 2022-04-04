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

			// Get shared pointers to entities
			shared_ptr<Entity> car1 = e1->shared_from_this();
			shared_ptr<Entity> car2 = e2->shared_from_this();

            if ((car1->hasComponent<VehicleComponent>()   && car2->hasComponent<RigidbodyComponent>())
            ||  (car1->hasComponent<RigidbodyComponent>() && car2->hasComponent<VehicleComponent>())
            ||  (car1->hasComponent<VehicleComponent>()   && car2->hasComponent<VehicleComponent>())) {
                glm::vec3 pos = car1->getComponent<TransformComponent>()->getGlobalPosition();
                Events::Collision.broadcast(car1, car2);
            }

        }
    }
}


void PhysXSimCallback::onTrigger(
	PxTriggerPair* pairs,
	PxU32 nbPairs)
{
	
	for (PxU32 i = 0; i < nbPairs; i++) {

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
			// pairs->triggerActor->getShapes(car->getComponent<VehicleComponent>()->triggerShapes, 1, 0);
		}

		// Activated when an AI collides with triggerbox
		if ((car->hasComponent<VehicleComponent>() && car->hasComponent<AiComponent>()) && (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)) {
			Events::CarParked.broadcast(car, trigger);
		}
		*/
	}
	
}


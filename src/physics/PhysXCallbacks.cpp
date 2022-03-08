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

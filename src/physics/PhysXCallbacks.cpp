#include "PhysXCallbacks.h"
#include "crapweactuallyneed.h"
#include "components/Entity.h"
#include "Event.h"

extern unsigned int g_boxID;
extern unsigned int g_carID;
extern unsigned int g_aiID;

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

            unsigned int eID_1 = e1->id();
            unsigned int eID_2 = e2->id();

            if (eID_1 == g_boxID || eID_2 == g_boxID) {
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
		/*if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
			PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;*/

		std::cout << "trigger" << std::endl;

	}

}


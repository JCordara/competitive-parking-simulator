#ifndef PHYSX_CALLBACKS_H
#define PHYSX_CALLBACKS_H

#include <PhysX/PxPhysicsAPI.h>
#include "Common.h"

using namespace physx;

class PhysXSimCallback : public PxSimulationEventCallback {
public:
	
    void onContact(
        const PxContactPairHeader& pairHeader,
        const PxContactPair* pairs, 
        PxU32 nbPairs);

	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
	void onWake(PxActor** actors, PxU32 count) {}
	void onSleep(PxActor** actors, PxU32 count) {}
	void onTrigger(PxTriggerPair* pairs, PxU32 nbPairs);
	void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}
    
};

#endif // PHYSX_CALLBACKS_H

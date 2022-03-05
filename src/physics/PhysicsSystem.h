#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "GameSystem.h"

#include <PhysX/PxPhysicsAPI.h>
#include "crapweactuallyneed.h"

using namespace physx;

class PhysicsSystem : public GameSystem {
public:

    friend PhysicsComponent;

    /* Prepare framework */
    PhysicsSystem(shared_ptr<Scene> scene);

    void update();

    void createConvexHull(std::vector<PxVec3> convexVerts);

    void PhysXMat4ToglmMat4(const PxMat44& mat4, glm::mat4& newMat);
	void PhysXVec3ToglmVec3(const PxVec3& vec3, glm::vec3& newVec);
    ~PhysicsSystem();
    
private:

    // Global physics engine things
    PxFoundation* pxFoundation;
    PxPhysics*    pxPhysics;
    PxScene*      pxScene;
    PxCooking*    pxCooking;
    PxPvd*        pxPvd;

    PxDefaultCpuDispatcher* pxDispatcher;

    PxDefaultAllocator	   pxAllocator;
    PxDefaultErrorCallback pxErrorCallback;

    shared_ptr<Scene> gameScene;

    VehicleDesc initVehicleDesc();
    void initPhysics();
    void stepPhysics();
    void cleanupPhysics();

    void initializePhysicsComponent(PhysicsComponent& component);

};


#endif // PHYSICS_SYSTEM_H

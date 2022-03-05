#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "GameSystem.h"

#include <PhysX/PxPhysicsAPI.h>
#include "crapweactuallyneed.h"

using namespace physx;

class PhysicsSystem : public GameSystem {
public:

    friend class RigidbodyComponent;
    friend class VehicleComponent;

    /* Prepare framework */
    PhysicsSystem(shared_ptr<Scene> scene);

    void update();

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

    // Default PhysX utilities
    PxDefaultCpuDispatcher* pxDispatcher;
    PxDefaultAllocator	    pxAllocator;
    PxDefaultErrorCallback  pxErrorCallback;

    // Drivable plane (tmp for Milestone 3)
    PxRigidStatic* pxGroundPlane;

    shared_ptr<Scene> gameScene;

    void initPhysX();

    void vehicleUpdate(shared_ptr<VehicleComponent>);
    void simulateScene();
    void cleanupPhysics();

    void vehicleAccelerateMode(Entity& entity, float v);
    void vehicleTurnMode(Entity& entity, float v);
    void vehicleBrakeMode(Entity& entity, float v);
    void vehicleHandbrakeMode(Entity& entity, float v);

    void registerRigidbodyComponent(RigidbodyComponent& component);
    void registerVehicleComponent(VehicleComponent& component);

};


#endif // PHYSICS_SYSTEM_H

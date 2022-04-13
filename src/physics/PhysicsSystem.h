#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "GameSystem.h"

#include <PhysX/PxPhysicsAPI.h>
#include "PhysXCallbacks.h"
#include "VehicleDesc.h"
#include "VehicleSceneQueryData.h"
#include "SnippetCode.h"

class Model;

using namespace physx;

class PhysicsSystem : public GameSystem {
public:

    friend class RigidbodyComponent;
    friend class VehicleComponent;
	friend class VolumeTriggerComponent;

    /* Prepare framework */
    PhysicsSystem(shared_ptr<Scene> scene);

    void update();

    PxTriangleMesh* createStaticMesh(const Model& model);
    PxSphereGeometry* createStaticSphere(const float radius);
    PxConvexMesh*   createDynamicMesh(const Model& model);
    PxConvexMesh*   createDynamicMesh(const PxVec3* v, const PxU32 n);
	PxRigidActor* createTriggerBox(PxTransform startPos, PxBoxGeometry boxGeom);
	PxRigidActor* createCylinderStatic(PxTransform startPos, PxVec3 dims);
	

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
    
    // Callback function for simulation events
    shared_ptr<PxSimulationEventCallback> pxSimEventCallback;

    // Drivable plane (tmp for Milestone 3)
    PxRigidStatic* pxGroundPlane;

    shared_ptr<Scene> gameScene;

    void initPhysX();

    void vehicleUpdate(weak_ptr<VehicleComponent>);
    void simulateScene();
	void cleanupPhysics();

    void vehicleAccelerateMode(weak_ptr<Entity> entity, float v);
    void vehicleTurnMode(weak_ptr<Entity> entity, float v);
    void vehicleBrakeMode(weak_ptr<Entity> entity, float v);
    void vehicleHandbrakeMode(weak_ptr<Entity> entity, float v);
	void vehicleFlipMode(weak_ptr<Entity> entity, float v);


    void registerRigidbodyComponent(RigidbodyComponent& component);
    void registerVehicleComponent(VehicleComponent& component);
	void registerVolumeTriggerComponent(VolumeTriggerComponent& component);

};


#endif // PHYSICS_SYSTEM_H

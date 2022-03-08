#ifndef VEHICLE_COMPONENT_H
#define VEHICLE_COMPONENT_H

#include "Components.h"
#include <PhysX/PxPhysicsAPI.h>

class PhysicsSystem;
struct VehicleDesc;

using namespace physx;

class VehicleComponent : public BaseComponent {
public:

    friend class PhysicsSystem;

    VehicleComponent(Entity& parent);
    ~VehicleComponent();
    static ComponentEnum getType();

    inline void setTransform(PxTransform& transform) {
        vehicle->getRigidDynamicActor()->setGlobalPose(transform);
    }

    inline void setPhysicsSystem(shared_ptr<PhysicsSystem> system) {
        physicsSystem = system;
    }

private:

    VehicleDesc initVehicleDesc();

    PxVehicleDrive4W* createVehicle4W(const VehicleDesc& vehicle4WDesc);

    PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius);
    
    PxRigidDynamic* createVehicleActor(
        const PxVehicleChassisData& chassisData, PxMaterial** wheelMaterials, 
        PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels, 
        const PxFilterData& wheelSimFilterData, PxMaterial** chassisMaterials, 
        PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, 
        const PxFilterData& chassisSimFilterData);


    shared_ptr<PhysicsSystem> physicsSystem;

    PxVehicleDrive4W* vehicle;
    PxVehicleDrive4WRawInputData inputData;
    bool isInAir;

    PxConvexMesh* chassisMesh;
    // PxConvexMesh* wheelMesh;

};

#endif // VEHICLE_COMPONENT_H

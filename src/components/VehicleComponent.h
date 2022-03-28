#ifndef VEHICLE_COMPONENT_H
#define VEHICLE_COMPONENT_H

#include "Components.h"
#include "LuaScript.h"
#include <PhysX/PxPhysicsAPI.h>

class PhysicsSystem;
struct VehicleDesc;

using namespace physx;

class VehicleComponent : public BaseComponent {
public:

	PxVehicleDrive4W* vehicle;
    PxShape* wheelShapes[4];
    sp<Entity>  wheelEntities[4];
	PxShape* triggerShapes[1];


    friend class PhysicsSystem;

    VehicleComponent(shared_ptr<Entity> parent);
    ~VehicleComponent();
    static ComponentEnum getType();

    float getSpeed();
    float getEngineSpeed();
    float getEngineSpeedNormalized();
    float getSkidSpeed();
    bool isGrounded();

    inline void setTransform(PxTransform& transform) {
        vehicle->getRigidDynamicActor()->setGlobalPose(transform);
    }

    inline void setPhysicsSystem(shared_ptr<PhysicsSystem> system) {
        physicsSystem = system;
    }

private:

    shared_ptr<PhysicsSystem> physicsSystem;

    PxVehicleDrive4WRawInputData inputData;
    bool isInAir_;
    int numWheels;

    PxConvexMesh* chassisMesh;

    Script tuningScript;

    PxVehicleEngineData engineData;
    PxVehicleWheelData  wheelData;
    PxVec3 chassisDims;
    double maxHandbrakeTorque;
    double maxSteer;
    double chassisMass;
    double CMoffsetX;
    double CMoffsetY;
    double CMoffsetZ;

    void reloadVehicleSettings();
    VehicleDesc initVehicleDesc();

    PxVehicleDrive4W* createVehicle4W(const VehicleDesc& vehicle4WDesc);

    PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius);
    
    PxRigidDynamic* createVehicleActor(
        const PxVehicleChassisData& chassisData, PxMaterial** wheelMaterials, 
        PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels, 
        const PxFilterData& wheelSimFilterData, PxMaterial** chassisMaterials, 
        PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, 
        const PxFilterData& chassisSimFilterData);

    void printTuningInfo();

};

#endif // VEHICLE_COMPONENT_H

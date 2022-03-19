#pragma once

#include <PhysX/PxPhysicsAPI.h>

#include "VehicleDesc.h"
#include "VehicleSceneQueryData.h"

class Entity;

using namespace physx;

#define PVD_HOST "127.0.0.1"
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}





extern PxMaterial* gMaterial;
extern VehicleSceneQueryData* gVehicleSceneQueryData;
extern PxBatchQuery* gBatchQuery;
extern PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs;

extern PxVehicleKeySmoothingData gKeySmoothingData;
extern PxVehiclePadSmoothingData gPadSmoothingData;
extern PxF32 gSteerVsForwardSpeedData[2 * 8];
extern PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable;

extern bool gMimicKeyInputs;

enum
{
    COLLISION_FLAG_GROUND            = 1 << 0,
    COLLISION_FLAG_WHEEL             = 1 << 1,
    COLLISION_FLAG_CHASSIS           = 1 << 2,
    COLLISION_FLAG_OBSTACLE          = 1 << 3,
    COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,

    COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_CHASSIS_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE
};


//Drivable surface types.
enum
{
    SURFACE_TYPE_TARMAC,
    MAX_NUM_SURFACE_TYPES
};

//Tire types.
enum
{
    TIRE_TYPE_NORMAL = 0,
    TIRE_TYPE_WORN,
    MAX_NUM_TIRE_TYPES
};

enum
{
    DRIVABLE_SURFACE = 0xffff0000,
    UNDRIVABLE_SURFACE = 0x0000ffff
};

//Tire model friction for each combination of drivable surface type and tire type.
static PxF32 gTireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
{
    //NORMAL,	WORN
    {2.00f,    0.1f}//TARMAC
};


PxFilterFlags VehicleFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

void setupDrivableSurface(PxFilterData& filterData);

void setupNonDrivableSurface(PxFilterData& filterData);

namespace fourwheel
{

    void computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 numWheels, PxVec3* wheelCentreOffsets);
    
    void setupWheelsSimulationData
    (const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
        const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
        const PxVec3& chassisCMOffset, const PxF32 chassisMass,
        PxVehicleWheelsSimData* wheelsSimData);

} //namespace fourwheel

void configureUserData(PxVehicleWheels* vehicle, ActorUserData* actorUserData, ShapeUserData* shapeUserDatas);

static PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking);

PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking);

PxConvexMesh* createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking);



PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs(const PxMaterial* defaultMaterial);

PxRigidStatic* createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics);


PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking
(PxFilterData filterData0, PxFilterData filterData1,
    const void* constantBlock, PxU32 constantBlockSize,
    PxHitFlags& queryFlags);


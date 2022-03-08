#pragma once

#include <PhysX/PxPhysicsAPI.h>

#include "VehicleDesc.h"

class Entity;

using namespace physx;

#define PVD_HOST "127.0.0.1"
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}


//Data structure for quick setup of scene queries for suspension queries.
class VehicleSceneQueryData
{
public:
    VehicleSceneQueryData();
    ~VehicleSceneQueryData();

    //Allocate scene query data for up to maxNumVehicles and up to maxNumWheelsPerVehicle with numVehiclesInBatch per batch query.
    static VehicleSceneQueryData* allocate
    (const PxU32 maxNumVehicles, const PxU32 maxNumWheelsPerVehicle, const PxU32 maxNumHitPointsPerWheel, const PxU32 numVehiclesInBatch,
        PxBatchQueryPreFilterShader preFilterShader, PxBatchQueryPostFilterShader postFilterShader,
        PxAllocatorCallback& allocator);

    //Free allocated buffers.
    void free(PxAllocatorCallback& allocator);

    //Create a PxBatchQuery instance that will be used for a single specified batch.
    static PxBatchQuery* setUpBatchedSceneQuery(const PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, PxScene* scene);

    //Return an array of scene query results for a single specified batch.
    PxRaycastQueryResult* getRaycastQueryResultBuffer(const PxU32 batchId);

    //Return an array of scene query results for a single specified batch.
    PxSweepQueryResult* getSweepQueryResultBuffer(const PxU32 batchId);

    //Get the number of scene query results that have been allocated for a single batch.
    PxU32 getQueryResultBufferSize() const;

private:

    //Number of queries per batch
    PxU32 mNumQueriesPerBatch;

    //Number of hit results per query
    PxU32 mNumHitResultsPerQuery;

    //One result for each wheel.
    PxRaycastQueryResult* mRaycastResults;
    PxSweepQueryResult* mSweepResults;

    //One hit for each wheel.
    PxRaycastHit* mRaycastHitBuffer;
    PxSweepHit* mSweepHitBuffer;

    //Filter shader used to filter drivable and non-drivable surfaces
    PxBatchQueryPreFilterShader mPreFilterShader;

    //Filter shader used to reject hit shapes that initially overlap sweeps.
    PxBatchQueryPostFilterShader mPostFilterShader;

};


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


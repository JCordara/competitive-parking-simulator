#include <ctype.h>

#include <PhysX/PxPhysicsAPI.h>

#include "PhysX/vehicle/PxVehicleUtil.h"
#include "SnippetVehicleSceneQuery.h"
#include "SnippetVehicleFilterShader.h"
#include "SnippetVehicleTireFriction.h"
#include "SnippetVehicleCreate.h"

#include "SnippetPrint.h"
#include "SnippetPVD.h"
#include "SnippetUtils.h"

using namespace physx;
using namespace snippetvehicle;

extern PxDefaultAllocator		gAllocator;
extern PxDefaultErrorCallback	gErrorCallback;

extern PxFoundation* gFoundation;
extern PxPhysics* gPhysics;

extern PxDefaultCpuDispatcher* gDispatcher;
extern PxScene* gScene;

extern PxCooking* gCooking;

extern PxMaterial* gMaterial;

extern PxPvd* gPvd;

extern VehicleSceneQueryData* gVehicleSceneQueryData;
extern PxBatchQuery* gBatchQuery;

extern PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs;

extern PxRigidStatic* gGroundPlane;
extern PxVehicleDrive4W* gVehicle4W;
extern PxRigidDynamic* box1;
extern PxRigidStatic* box2;
extern PxRigidStatic* box3;

extern bool	gIsVehicleInAir;


extern PxF32 gSteerVsForwardSpeedData[];
extern PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable;

extern PxVehicleKeySmoothingData gKeySmoothingData;
extern PxVehiclePadSmoothingData gPadSmoothingData;

extern PxVehicleDrive4WRawInputData gVehicleInputData;


enum DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

DriveMode gDriveModeOrder[];

extern PxF32					gVehicleModeLifetime;
extern PxF32					gVehicleModeTimer;
extern PxU32					gVehicleOrderProgress;
extern bool					gVehicleOrderComplete;
extern bool					gMimicKeyInputs;


void vehicleAccelerateMode(float);
void vehicleTurnMode(float);
void vehicleBrakeMode(float);
void vehicleHandbrakeMode(float);

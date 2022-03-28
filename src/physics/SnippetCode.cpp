#pragma once

#include "SnippetCode.h"

PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxMaterial* gMaterial = NULL;
bool gMimicKeyInputs = false;

PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};


PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxFilterFlags VehicleFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    PX_UNUSED(attributes0);
    PX_UNUSED(attributes1);
    PX_UNUSED(constantBlock);
    PX_UNUSED(constantBlockSize);

	//let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

    pairFlags = PxPairFlag::eCONTACT_DEFAULT;
    
    if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

    if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
        return PxFilterFlag::eSUPPRESS;

    pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));

    return PxFilterFlags();
}


void setupDrivableSurface(PxFilterData& filterData)
{
    filterData.word3 = static_cast<PxU32>(DRIVABLE_SURFACE);
}

void setupNonDrivableSurface(PxFilterData& filterData)
{
    filterData.word3 = UNDRIVABLE_SURFACE;
}



namespace fourwheel
{

    void computeWheelCenterActorOffsets4W(
        const PxF32 wheelFrontZ, 
        const PxF32 wheelRearZ, 
        const PxVec3& chassisDims, 
        const PxF32 wheelWidth, 
        const PxF32 wheelRadius, 
        const PxU32 numWheels, 
        PxVec3* wheelCentreOffsets)
    {

        //chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
        //The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
        //Compute a position for the front wheel and the rear wheel along the z-axis.
        //Compute the separation between each wheel along the z-axis.
        const PxF32 numLeftWheels = numWheels / 2.0f;
        const PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f);

        // Calculate where to put the wheels in order to lift the chassis the specified amount
        const PxF32 lift = 0.01f;
        const PxF32 height = (-0.5 * chassisDims.y) + (wheelRadius * 1.3f) - lift;

        //Set the outside of the left and right wheels to be flush with the chassis.
        //Set the top of the wheel to be just touching the underside of the chassis.
        //Begin by setting the rear-left/rear-right/front-left,front-right wheels.
        wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT] = PxVec3(
            -(0.5*chassisDims.x) + (wheelWidth * 0.8f), 
            height,
            -(0.5*chassisDims.z) + 0.625f
        );
        wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = PxVec3(
            (0.5*chassisDims.x) - (wheelWidth * 0.8f), 
            height,
            -(0.5*chassisDims.z) + 0.625f 
        );
        wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = PxVec3(
            -(0.5*chassisDims.x) + (wheelWidth * 0.8f), 
            height,
            (0.5*chassisDims.z) - 0.64f
        );
        wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = PxVec3(
            (0.5*chassisDims.x) - (wheelWidth * 0.8f), 
            height,
            (0.5*chassisDims.z) - 0.64f
        );
        //Set the remaining wheels.
        for (PxU32 i = 2, wheelCount = 4; i < numWheels - 2; i += 2, wheelCount += 2)
        {
            wheelCentreOffsets[wheelCount + 0] = PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
            wheelCentreOffsets[wheelCount + 1] = PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
        }
    }

    void setupWheelsSimulationData
    (const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
        const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
        const PxVec3& chassisCMOffset, const PxF32 chassisMass,
        PxVehicleWheelsSimData* wheelsSimData)
    {
        //Set up the wheels.
        PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
        {
            //Set up the wheel data structures with mass, moi, radius, width.
            for (PxU32 i = 0; i < numWheels; i++)
            {
                wheels[i].mMass = wheelMass;
                wheels[i].mMOI = wheelMOI;
                wheels[i].mRadius = wheelRadius;
                wheels[i].mWidth = wheelWidth;
            }

            //Enable the handbrake for the rear wheels only.
            wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
            wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
            //Enable steering for the front wheels only.
            wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi * 0.3333f;
            wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi * 0.3333f;
        }

        //Set up the tires.
        PxVehicleTireData tires[PX_MAX_NB_WHEELS];
        {
            //Set up the tires.
            for (PxU32 i = 0; i < numWheels; i++)
            {
                tires[i].mType = TIRE_TYPE_NORMAL;
            }
        }

        //Set up the suspensions
        PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
        {
            //Compute the mass supported by each suspension spring.
            PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
            PxVehicleComputeSprungMasses
            (numWheels, wheelCenterActorOffsets,
                chassisCMOffset, chassisMass, 1, suspSprungMasses);

            //Set the suspension data.
            for (PxU32 i = 0; i < numWheels; i++)
            {
                suspensions[i].mMaxCompression = 0.38f;
                suspensions[i].mMaxDroop = 0.18f;
                suspensions[i].mSpringStrength = 37500.0f;
                suspensions[i].mSpringDamperRate = 3000.0f;
                suspensions[i].mSprungMass = suspSprungMasses[i];
            }

            //Set the camber angles.
            const PxF32 camberAngleAtRest = 0.0;
            const PxF32 camberAngleAtMaxDroop = 0.01f;
            const PxF32 camberAngleAtMaxCompression = -0.01f;
            for (PxU32 i = 0; i < numWheels; i += 2)
            {
                suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
                suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
                suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
                suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
                suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
                suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
            }
        }

        //Set up the wheel geometry.
        PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
        PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
        PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
        PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
        {
            //Set the geometry data.
            for (PxU32 i = 0; i < numWheels; i++)
            {
                //Vertical suspension travel.
                suspTravelDirections[i] = PxVec3(0, -1, 0);

                //Wheel center offset is offset from rigid body center of mass.
                wheelCentreCMOffsets[i] =
                    wheelCenterActorOffsets[i] - chassisCMOffset;

                //Suspension force application point 0.3 metres below 
                //rigid body center of mass.
                suspForceAppCMOffsets[i] =
                    PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

                //Tire force application point 0.3 metres below 
                //rigid body center of mass.
                tireForceAppCMOffsets[i] =
                    PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
            }
        }

        //Set up the filter data of the raycast that will be issued by each suspension.
        PxFilterData qryFilterData;
        setupNonDrivableSurface(qryFilterData);

        //Set the wheel, tire and suspension data.
        //Set the geometry data.
        //Set the query filter data
        for (PxU32 i = 0; i < numWheels; i++)
        {
            wheelsSimData->setWheelData(i, wheels[i]);
            wheelsSimData->setTireData(i, tires[i]);
            wheelsSimData->setSuspensionData(i, suspensions[i]);
            wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
            wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
            wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
            wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
            wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
            wheelsSimData->setWheelShapeMapping(i, PxI32(i));
        }

        //Add a front and rear anti-roll bar
        PxVehicleAntiRollBarData barFront;
        barFront.mWheel0 = PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
        barFront.mWheel1 = PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
        barFront.mStiffness = 10000.0f;
        wheelsSimData->addAntiRollBarData(barFront);
        PxVehicleAntiRollBarData barRear;
        barRear.mWheel0 = PxVehicleDrive4WWheelOrder::eREAR_LEFT;
        barRear.mWheel1 = PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
        barRear.mStiffness = 10000.0f;
        wheelsSimData->addAntiRollBarData(barRear);
    }

} //namespace fourwheel

void configureUserData(PxVehicleWheels* vehicle, ActorUserData* actorUserData, ShapeUserData* shapeUserDatas)
{
    if (actorUserData)
    {
        vehicle->getRigidDynamicActor()->userData = actorUserData;
        actorUserData->vehicle = vehicle;
    }

    if (shapeUserDatas)
    {
        PxShape* shapes[PX_MAX_NB_WHEELS + 1];
        vehicle->getRigidDynamicActor()->getShapes(shapes, PX_MAX_NB_WHEELS + 1);
        for (PxU32 i = 0; i < vehicle->mWheelsSimData.getNbWheels(); i++)
        {
            const PxI32 shapeId = vehicle->mWheelsSimData.getWheelShapeMapping(i);
            shapes[shapeId]->userData = &shapeUserDatas[i];
            shapeUserDatas[i].isWheel = true;
            shapeUserDatas[i].wheelId = i;
        }
    }
}


static PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking)
{
    // Create descriptor for convex mesh
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = numVerts;
    convexDesc.points.stride = sizeof(PxVec3);
    convexDesc.points.data = verts;
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

    PxConvexMesh* convexMesh = NULL;
    PxDefaultMemoryOutputStream buf;
    if (cooking.cookConvexMesh(convexDesc, buf))
    {
        PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
        convexMesh = physics.createConvexMesh(id);
    }

    return convexMesh;
}

PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking)
{
    PxVec3 points[2 * 16];
    for (PxU32 i = 0; i < 16; i++)
    {
        const PxF32 cosTheta = PxCos(i * PxPi * 2.0f / 16.0f);
        const PxF32 sinTheta = PxSin(i * PxPi * 2.0f / 16.0f);
        const PxF32 y = radius * cosTheta;
        const PxF32 z = radius * sinTheta;
        points[2 * i + 0] = PxVec3(-width / 2.0f, y, z);
        points[2 * i + 1] = PxVec3(+width / 2.0f, y, z);
    }

    return createConvexMesh(points, 32, physics, cooking);
}

PxConvexMesh* createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking)
{
    const PxF32 x = dims.x * 0.5f;
    const PxF32 y = dims.y * 0.5f;
    const PxF32 z = dims.z * 0.5f;
    PxVec3 verts[8] =
    {
        PxVec3(x,y,-z),
        PxVec3(x,y,z),
        PxVec3(x,-y,z),
        PxVec3(x,-y,-z),
        PxVec3(-x,y,-z),
        PxVec3(-x,y,z),
        PxVec3(-x,-y,z),
        PxVec3(-x,-y,-z)
    };

    return createConvexMesh(verts, 8, physics, cooking);
}


PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs(const PxMaterial* defaultMaterial)
{
    PxVehicleDrivableSurfaceType surfaceTypes[1];
    surfaceTypes[0].mType = SURFACE_TYPE_TARMAC;

    const PxMaterial* surfaceMaterials[1];
    surfaceMaterials[0] = defaultMaterial;

    PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
        PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES);

    surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES, surfaceMaterials, surfaceTypes);

    for (PxU32 i = 0; i < MAX_NUM_SURFACE_TYPES; i++)
    {
        for (PxU32 j = 0; j < MAX_NUM_TIRE_TYPES; j++)
        {
            surfaceTirePairs->setTypePairFriction(i, j, gTireFrictionMultipliers[i][j]);
        }
    }
    return surfaceTirePairs;
}

PxRigidStatic* createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics)
{
    //Add a plane to the scene.
    PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 1, 0, 1), *material);

    //Get the plane shape so we can set query and simulation filter data.
    PxShape* shapes[1];
    groundPlane->getShapes(shapes, 1);

    //Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
    PxFilterData qryFilterData;
    setupDrivableSurface(qryFilterData);
    shapes[0]->setQueryFilterData(qryFilterData);

    //Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
    shapes[0]->setSimulationFilterData(simFilterData);

    return groundPlane;
}


PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking
(PxFilterData filterData0, PxFilterData filterData1,
    const void* constantBlock, PxU32 constantBlockSize,
    PxHitFlags& queryFlags)
{
    //filterData0 is the vehicle suspension query.
    //filterData1 is the shape potentially hit by the query.
    PX_UNUSED(filterData0);
    PX_UNUSED(constantBlock);
    PX_UNUSED(constantBlockSize);
    PX_UNUSED(queryFlags);
    return ((0 == (filterData1.word3 & DRIVABLE_SURFACE)) ? PxQueryHitType::eNONE : PxQueryHitType::eBLOCK);
}


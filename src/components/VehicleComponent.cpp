#include "VehicleComponent.h"

#include "physics/PhysicsSystem.h"
#include "physics/VehicleDesc.h"


VehicleComponent::VehicleComponent(Entity& parent) 
    : BaseComponent(parent)
{

    // Notify Physics system that this component was created
    // This sets the reference to the physics system
    Events::VehicleComponentInit.broadcast(*this);

	// Get position of parent entity
	PxTransform entityPose = PxTransform(PxIdentity);

	if (auto tc = entity.getComponent<TransformComponent>()) {
		entityPose.p.x = tc->getLocalPosition().x;
		entityPose.p.y = tc->getLocalPosition().y;
		entityPose.p.z = tc->getLocalPosition().z;
		entityPose.q   = tc->getLocalRotation();
	}

	// Create chassis from entity's model if it has one
	if (auto mc = entity.getComponent<ModelComponent>()) {
		chassisMesh = physicsSystem->createDynamicMesh(*mc->getModel());
	} else {
		// Default chassis dimensions
		const PxF32 x = 1.25f;
		const PxF32 y = 1.0f;
		const PxF32 z = 2.5f;

		PxVec3 verts[8] = {
			PxVec3( x, y,-z), PxVec3( x, y, z), PxVec3( x,-y, z),
			PxVec3( x,-y,-z), PxVec3(-x, y,-z), PxVec3(-x, y, z),
			PxVec3(-x,-y, z), PxVec3(-x,-y,-z)
		};

		// Create default chassis
		chassisMesh = physicsSystem->createDynamicMesh(verts, 8);
	}

    // Create a vehicle
	VehicleDesc vehicleDesc = initVehicleDesc();
	vehicle = createVehicle4W(vehicleDesc);

    // Add vehicle to physics scene
	vehicle->getRigidDynamicActor()->setGlobalPose(entityPose);
	physicsSystem->pxScene->addActor(*vehicle->getRigidDynamicActor());
}

VehicleDesc VehicleComponent::initVehicleDesc()
{
	// Set up the chassis mass, dimensions, moment of inertia, and center of mass offset
	// The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering
	// Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims = chassisMesh->getLocalBounds().getDimensions();
	const PxVec3 chassisMOI(
		(chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.75f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = gMaterial;
	vehicleDesc.wheelSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

PxConvexMesh* VehicleComponent::createWheelMesh(
	const PxF32 width, const PxF32 radius) 
{
	const unsigned int n = 2 * 16;
    PxVec3 points[n];

    for (PxU32 i = 0; i < (n / 2); i++) {
        const PxF32 cosTheta = PxCos(i * PxPi * 4.0f / n);
        const PxF32 sinTheta = PxSin(i * PxPi * 4.0f / n);
        const PxF32 y = radius * cosTheta;
        const PxF32 z = radius * sinTheta;
        points[2 * i + 0] = PxVec3(-width / 2.0f, y, z);
        points[2 * i + 1] = PxVec3(+width / 2.0f, y, z);
    }

	PxConvexMesh* convexMesh = physicsSystem->createDynamicMesh(points, n);

    return convexMesh;
}


PxRigidDynamic* VehicleComponent::createVehicleActor(
    const PxVehicleChassisData& chassisData, PxMaterial** wheelMaterials, 
    PxConvexMesh** wheelConvexMeshes, 	     const PxU32 numWheels, 
    const PxFilterData& wheelSimFilterData,  PxMaterial** chassisMaterials, 
    PxConvexMesh** chassisConvexMeshes,      const PxU32 numChassisMeshes, 
    const PxFilterData& chassisSimFilterData)
{

    //We need a rigid body actor for the vehicle.
    //Don't forget to add the actor to the scene after setting up the associated vehicle.
    PxRigidDynamic* vehActor = physicsSystem->pxPhysics->createRigidDynamic(PxTransform(PxIdentity));

    //Wheel and chassis query filter data.
    //Optional: cars don't drive on other cars.
    PxFilterData wheelQryFilterData;
    setupNonDrivableSurface(wheelQryFilterData);
    PxFilterData chassisQryFilterData;
    setupNonDrivableSurface(chassisQryFilterData);

    //Add all the wheel shapes to the actor.
    for (PxU32 i = 0; i < numWheels; i++)
    {
        PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
        PxShape* wheelShape = PxRigidActorExt::createExclusiveShape(*vehActor, geom, *wheelMaterials[i]);
        wheelShape->setQueryFilterData(wheelQryFilterData);
        wheelShape->setSimulationFilterData(wheelSimFilterData);
        wheelShape->setLocalPose(PxTransform(PxIdentity));
    }

    //Add the chassis shapes to the actor.
    for (PxU32 i = 0; i < numChassisMeshes; i++)
    {
        PxShape* chassisShape = PxRigidActorExt::createExclusiveShape(*vehActor, PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]);

        chassisShape->setQueryFilterData(chassisQryFilterData);
        chassisShape->setSimulationFilterData(chassisSimFilterData);
        chassisShape->setLocalPose(PxTransform(PxIdentity));
    }

    vehActor->setMass(chassisData.mMass);
    vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
    vehActor->setCMassLocalPose(PxTransform(chassisData.mCMOffset, PxQuat(PxIdentity)));

    return vehActor;
}


PxVehicleDrive4W* VehicleComponent::createVehicle4W(const VehicleDesc& vehicle4WDesc)
{
    const PxVec3 chassisDims = vehicle4WDesc.chassisDims;
    const PxF32 wheelWidth = vehicle4WDesc.wheelWidth;
    const PxF32 wheelRadius = vehicle4WDesc.wheelRadius;
    const PxU32 numWheels = vehicle4WDesc.numWheels;

    const PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
    const PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

    //Construct a physx actor with shapes for the chassis and wheels.
    //Set the rigid body mass, moment of inertia, and center of mass offset.
    PxRigidDynamic* veh4WActor = nullptr;
    {
        //Construct a convex mesh for a cylindrical wheel.
        PxConvexMesh* wheelMesh = createWheelMesh(wheelWidth, wheelRadius);
        //Assume all wheels are identical for simplicity.
        PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
        PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

        //Set the meshes and materials for the driven wheels.
        for (PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
        {
            wheelConvexMeshes[i] = wheelMesh;
            wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
        }
        //Set the meshes and materials for the non-driven wheels
        for (PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
        {
            wheelConvexMeshes[i] = wheelMesh;
            wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
        }

        PxConvexMesh* chassisConvexMeshes[1] = { chassisMesh };
        PxMaterial* chassisMaterials[1] = { vehicle4WDesc.chassisMaterial };

        //Rigid body data.
        PxVehicleChassisData rigidBodyData;
        rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
        rigidBodyData.mMass = vehicle4WDesc.chassisMass;
        rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

        veh4WActor = createVehicleActor(
			rigidBodyData, wheelMaterials, wheelConvexMeshes, numWheels, 
			wheelSimFilterData, chassisMaterials, chassisConvexMeshes, 1, 
			chassisSimFilterData
		);
    }

    //Set up the sim data for the wheels.
    PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(numWheels);
    {
        //Compute the wheel center offsets from the origin.
        PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
        const PxF32 frontZ = chassisDims.z * 0.3f;
        const PxF32 rearZ = -chassisDims.z * 0.3f;
        fourwheel::computeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels, wheelCenterActorOffsets);

        //Set up the simulation data for all wheels.
        fourwheel::setupWheelsSimulationData
        (vehicle4WDesc.wheelMass, vehicle4WDesc.wheelMOI, wheelRadius, wheelWidth,
            numWheels, wheelCenterActorOffsets,
            vehicle4WDesc.chassisCMOffset, vehicle4WDesc.chassisMass,
            wheelsSimData);
    }

    //Set up the sim data for the vehicle drive model.
    PxVehicleDriveSimData4W driveSimData;
    {
        //Diff
        PxVehicleDifferential4WData diff;
        diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
        driveSimData.setDiffData(diff);

        //Engine
        PxVehicleEngineData engine;
        engine.mPeakTorque = 1200.0f;  // accel
        engine.mMaxOmega = 600.0f;     // top speed
        engine.mMOI = 0.15f;
        engine.mDampingRateFullThrottle = 0.25f;
        engine.mDampingRateZeroThrottleClutchEngaged = 3.0f;
        driveSimData.setEngineData(engine);

        //Gears
        PxVehicleGearsData gears;
        gears.mSwitchTime = 0.5f;
        driveSimData.setGearsData(gears);

        //Clutch
        PxVehicleClutchData clutch;
        clutch.mStrength = 10.0f;
        driveSimData.setClutchData(clutch);

        //Ackermann steer accuracy
        PxVehicleAckermannGeometryData ackermann;
        ackermann.mAccuracy = 1.0f;
        ackermann.mAxleSeparation =
            wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
            wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
        ackermann.mFrontWidth =
            wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
            wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
        ackermann.mRearWidth =
            wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
            wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
        driveSimData.setAckermannGeometryData(ackermann);
    }

    //Create a vehicle from the wheels and drive sim data.
    PxVehicleDrive4W* vehDrive4W = PxVehicleDrive4W::allocate(numWheels);
    vehDrive4W->setup(physicsSystem->pxPhysics, veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);
	
    //Configure the userdata
    configureUserData(vehDrive4W, vehicle4WDesc.actorUserData, vehicle4WDesc.shapeUserDatas);

    vehDrive4W->getRigidDynamicActor()->userData = &entity;

    //Free the sim data because we don't need that any more.
    wheelsSimData->free();

    return vehDrive4W;
}


ComponentEnum VehicleComponent::getType() {
    return ComponentEnum::vehicle;
}

VehicleComponent::~VehicleComponent() {
    // Nothing to do here yet
}

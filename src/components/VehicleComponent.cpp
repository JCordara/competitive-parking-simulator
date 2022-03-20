#include "VehicleComponent.h"

#include "Physics/PhysicsSystem.h"
#include "Physics/VehicleDesc.h"


VehicleComponent::VehicleComponent(shared_ptr<Entity> parent): 
    BaseComponent(parent),
    numWheels(0),
    chassisMass(1500.0f),
    tuningScript("scripts/vehicleTuning.lua")
{

    // Notify Physics system that this component was created
    // This sets the reference to the physics system
    Events::VehicleComponentInit.broadcast(*this);

	// Get position of parent entity
	PxTransform entityPose = PxTransform(PxIdentity);

	if (auto tc = entity->getComponent<TransformComponent>()) {
		entityPose.p.x = tc->getLocalPosition().x;
		entityPose.p.y = tc->getLocalPosition().y;
		entityPose.p.z = tc->getLocalPosition().z;
		entityPose.q   = tc->getLocalRotation();
	}

	// Create chassis from entity's model if it has one
	if (auto mc = entity->getComponent<ModelComponent>()) {
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

    // Get wheel children on component creation
    for (auto& child : entity->directChildren()) {
        if (auto desc = child->getComponent<DescriptionComponent>()) {
            if (*desc->getInteger("wheel")) wheelEntities[numWheels++] = child;
        }
    }

    // Create a vehicle
	VehicleDesc vehicleDesc = initVehicleDesc();
	vehicle = createVehicle4W(vehicleDesc);

    // Add vehicle to physics scene
	vehicle->getRigidDynamicActor()->setGlobalPose(entityPose);
	physicsSystem->pxScene->addActor(*vehicle->getRigidDynamicActor());

    // Get wheel positions
	PxShape *shapes[8];
	vehicle->getRigidDynamicActor()->getShapes(shapes, 8, 0);
	for (int i = 0; i < 4; i++) {
		wheelShapes[i] = shapes[vehicle->mWheelsSimData.getWheelShapeMapping(i)];
	}

    // Set up tuning script
    tuningScript.setGlobal(chassisDims.x, "_chassisDimensionsX");
    tuningScript.setGlobal(chassisDims.y, "_chassisDimensionsY");
    tuningScript.setGlobal(chassisDims.z, "_chassisDimensionsZ");
    tuningScript.setGlobal(chassisMass, "mass");
    tuningScript.setGlobal(CMoffsetX, "CMoffsetX");
    tuningScript.setGlobal(CMoffsetY, "CMoffsetY");
    tuningScript.setGlobal(CMoffsetZ, "CMoffsetZ");

    tuningScript.setGlobal(engineData.mMaxOmega, "speed");
    tuningScript.setGlobal(engineData.mPeakTorque, "torque");
    tuningScript.setGlobal(engineData.mMOI, "engineMOI");
    tuningScript.setGlobal(engineData.mDampingRateFullThrottle, "dampingFullThrottle");
    tuningScript.setGlobal(engineData.mDampingRateZeroThrottleClutchEngaged, "dampingClutchEngaged");
    tuningScript.setGlobal(engineData.mDampingRateZeroThrottleClutchDisengaged, "dampingClutchDisengaged");
    
    tuningScript.setGlobal(wheelData.mRadius, "_wheelRadius");
    tuningScript.setGlobal(maxHandbrakeTorque, "handbrakeTorque");
    tuningScript.setGlobal(maxSteer, "turningRadius");
    tuningScript.setGlobal(wheelData.mMass, "wheelMass");
    tuningScript.setGlobal(wheelData.mMOI, "wheelMOI");
    tuningScript.setGlobal(wheelData.mDampingRate, "wheelDamping");
    tuningScript.setGlobal(wheelData.mToeAngle, "toeAngle");

    // Set initial vehicle values
    tuningScript.run();
    reloadVehicleSettings();

    // Reload vehicle tuning settings when script is recompiled
    Events::RecompileScripts.registerHandler<VehicleComponent,
        &VehicleComponent::reloadVehicleSettings>(this);
}

VehicleDesc VehicleComponent::initVehicleDesc()
{
	// Set up the chassis mass, dimensions, moment of inertia, and center of mass offset
	// The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering
	// Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front
	chassisDims = chassisMesh->getLocalBounds().getDimensions();
	const PxVec3 chassisMOI(
		(chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);

    CMoffsetX = 0.0f;
    CMoffsetY = -chassisDims.y * 0.5f + 0.75f;
    CMoffsetZ = 0.25f;
	const PxVec3 chassisCMOffset(CMoffsetX, CMoffsetY, CMoffsetZ);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
    const PxF32 wheelWidth = 0.4f;
    const PxF32 wheelRadius = 0.5f;

    if (wheelEntities[0]) {
        auto wheelModel = wheelEntities[0]->getComponent<ModelComponent>()->getModel();
        PxVec3 wheelDimensions = physicsSystem->createDynamicMesh(*wheelModel)->getLocalBounds().getDimensions();

        const PxF32 wheelWidth = wheelDimensions.x;
        const PxF32 wheelRadius = wheelDimensions.y / 2;
    }

    const PxF32 wheelMass = 20.0f;
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

    wheelData.mMass = wheelMass;
    wheelData.mMOI = wheelMOI;
    wheelData.mRadius = wheelRadius;
    wheelData.mWidth = wheelWidth;

    engineData.mPeakTorque = 1200.0f;  // accel
    engineData.mMaxOmega = 600.0f;     // top speed
    engineData.mMOI = 0.15f;
    engineData.mDampingRateFullThrottle = 0.25f;
    engineData.mDampingRateZeroThrottleClutchEngaged = 3.0f;

	return vehicleDesc;
}

void VehicleComponent::reloadVehicleSettings() 
{    
    // Run tuning script
    tuningScript.run();

    // Reload center of mass
    PxTransform CMoffset(CMoffsetX, CMoffsetY, CMoffsetZ);
    vehicle->getRigidDynamicActor()->setCMassLocalPose(CMoffset);

    // Reload chassis mass
    vehicle->mWheelsSimData.setChassisMass(chassisMass);

    // Reload engine data
    vehicle->mDriveSimData.setEngineData(engineData);

    // Reload wheel data
    PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
    {
        //Set up the wheel data structures with mass, moi, radius, width.
        for (PxU32 i = 0; i < numWheels; i++) wheels[i] = wheelData;

        //Enable the handbrake for the rear wheels only.
        wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = maxHandbrakeTorque;
        wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = maxHandbrakeTorque;
        //Enable steering for the front wheels only.
        wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi * maxSteer;
        wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi * maxSteer;

        // Set each wheels data
        for (PxU32 i = 0; i < numWheels; i++)
            vehicle->mWheelsSimData.setWheelData(i, wheels[i]);
    }

    // printTuningInfo();

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
    vehActor->setCMassLocalPose(PxTransform(CMoffsetX, CMoffsetY, CMoffsetZ));

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
        // Check if the car has explicitly created wheels
        bool defaultWheels = static_cast<bool>(wheelEntities[0]);

        // Construct a convex mesh for a cylindrical wheel
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

    vehDrive4W->getRigidDynamicActor()->userData = static_cast<void*>(entity.get());

    //Free the sim data because we don't need that any more.
    wheelsSimData->free();

    return vehDrive4W;
}

void VehicleComponent::printTuningInfo() {
    // Get info from vehicle
    PxRigidDynamic* actor = vehicle->getRigidDynamicActor();
    PxVec3 CMoffset = actor->getCMassLocalPose().p;
    PxVehicleEngineData engine = vehicle->mDriveSimData.getEngineData();
    PxVehicleWheelData wheelsF = vehicle->mWheelsSimData.getWheelData(PxVehicleDrive4WWheelOrder::eFRONT_LEFT);
    PxVehicleWheelData wheelsR = vehicle->mWheelsSimData.getWheelData(PxVehicleDrive4WWheelOrder::eREAR_LEFT);

    // Print info
    printf("Mass: %.2f\n", actor->getMass());
    printf("CM offset: (%.2f, %.2f, %.2f)\n", CMoffset.x, CMoffset.y, CMoffset.z);
    printf("Speed: %.2f\n", engine.mMaxOmega);
    printf("Torque: %.2f\n", engine.mPeakTorque);
    printf("Engine MOI: %.2f\n", engine.mMOI);
    printf("Engine damping (Full throttle): %.2f\n", engine.mDampingRateFullThrottle);
    printf("Engine damping (Clutch engaged): %.2f\n", engine.mDampingRateZeroThrottleClutchEngaged);
    printf("Engine damping (Clutch disengaged): %.2f\n", engine.mDampingRateZeroThrottleClutchDisengaged);
    printf("Handbrake torque: %.2f\n", wheelsR.mMaxHandBrakeTorque);
    printf("Turning radius: %.2f\n", wheelsF.mMaxSteer);
    printf("Wheel mass: %.2f\n", wheelsF.mMass);
    printf("Wheel MOI: %.2f\n", wheelsF.mMOI);
    printf("Wheel damping: %.2f\n", wheelsF.mDampingRate);
    printf("Wheel toe angle: %.2f\n", wheelsF.mToeAngle);
}


ComponentEnum VehicleComponent::getType() {
    return ComponentEnum::vehicle;
}

VehicleComponent::~VehicleComponent() {
    // Nothing to do here yet
}

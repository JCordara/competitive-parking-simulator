#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<AudioManager> audio)
    : scene(scene)
    , audioManager(audio)
{
    /* PhysX initialization */
    initPhysics();
	gVehicleInputData.setDigitalBrake(false);
}

void PhysicsSystem::update() {
    /* PhysX per-frame updates */
    stepPhysics();
}

/** Convert PhysX::PxMat44 to glm::mat4
 @param[in] mat4 The PhysX::PxMat44
 @param[ou] Converted glm::mat4
 */
void PhysicsSystem::PhysXMat4ToglmMat4(const PxMat44& mat4, glm::mat4& newMat)
{
	newMat[0][0] = -mat4[0][0];
	newMat[0][1] = mat4[0][1];
	newMat[0][2] = mat4[0][2];
	newMat[0][3] = mat4[0][3];

	newMat[1][0] = -mat4[1][0];
	newMat[1][1] = mat4[1][1];
	newMat[1][2] = mat4[1][2];
	newMat[1][3] = mat4[1][3];

	newMat[2][0] = -mat4[2][0];
	newMat[2][1] = mat4[2][1];
	newMat[2][2] = mat4[2][2];
	newMat[2][3] = mat4[2][3];

	newMat[3][0] = -mat4[3][0];
	newMat[3][1] = mat4[3][1];
	newMat[3][2] = mat4[3][2];
	newMat[3][3] = mat4[3][3];

	newMat = glm::scale(newMat, glm::vec3(-1.f,1.f,1.f));
}

/** Convert Physx::PxVec3 to glm::vec3
 @param[in] vec3 The Physx::PxVec3
 @param[ou] Converted glm::mat4
 */
void PhysicsSystem::PhysXVec3ToglmVec3(const PxVec3& vec3, glm::vec3& newVec)
{
	newVec = glm::vec3(-vec3.x, vec3.y, vec3.z);
}

PhysicsSystem::~PhysicsSystem() {
    
}


void PhysicsSystem::stepPhysics()
{
	const PxF32 timestep = Time::timeStep();

	//Update the control inputs for the vehicle.
	if (gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}

	// When reversing while car is moving forwards, brake instead
	PxReal speed = gVehicle4W->computeForwardSpeed();
	PxReal input = gVehicle4W->mDriveDynData.getAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL);
	if (speed > 0.0 && input < 0.0) 
		gVehicleInputData.setAnalogBrake(-input);
	else
		gVehicleInputData.setAnalogBrake(0.0f);

	//Raycasts.
	PxVehicleWheels* vehicles[1] = { gVehicle4W };
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	//Scene update.
	gScene->simulate(timestep);
	gScene->fetchResults(true);
}


VehicleDesc PhysicsSystem::initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

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
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}


void PhysicsSystem::initPhysics()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	/////////////////////////////////////////////

	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gScene->addActor(*gGroundPlane);

	/*
	//Section for loading a heightmap as a drivable surface
	//some code gotten from https://gamedev.stackexchange.com/questions/86186/how-to-load-and-render-physx-3-3-heightfield
	int numRows = 16; int numCols = 16; //Example numbers, will be height map dimensions I believe
	// numRows & numCols are predifined
	// Putting malloc though original example had alloc, could not find the function
	PxHeightFieldSample* samples = (PxHeightFieldSample*)malloc(sizeof(PxHeightFieldSample) * (numRows * numCols));
	//To tell the system the number of sampled heights in each direction, use a descriptor to instantiate a PxHeightField object:
	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = numCols;
	hfDesc.nbRows = numRows;
	hfDesc.samples.data = samples;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);
	PxHeightField* aHeightField = gCooking->createHeightField(hfDesc, gPhysics->getPhysicsInsertionCallback());
	//The row and column scales tell the system how far apart the sampled points lie in the associated direction
	//The height scale scales the integer height values to a floating point range
	PxHeightFieldGeometry hfGeom(aHeightField, PxMeshGeometryFlags(), heightScale, rowScale, colScale);
	//The variant of createExclusiveShape() used here specifies an array of materials for the height field, which will be indexed by
	//the material indices of each cell to resolve collisions with that cell. The single-material variant may be used instead, but
	//the height field material indices must all be a single value or the special value eHOLE.
	// Actor might be a PxRigidStatic*??
	// PxRigidActor& actor, const PxGeometry& geometry, PxMaterial*const* materials, PxU16 materialCount,PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE)
	PxTransform pose(PxVec3(-((PxReal)numRows * rowScale) / 2.0f, 0.0f, -((PxReal)numCols * colScale) / 2.0f), PxQuat(PxIdentity));
	PxRigidActor* hf = gPhysics->createRigidStatic(pose);
	PxShape* aHeightFieldShape = PxRigidActorExt::createExclusiveShape(*hf, hfGeom, *gMaterial);
	PxFilterData qryFilterData2;
	setupDrivableSurface(qryFilterData2);
	aHeightFieldShape->setQueryFilterData(qryFilterData2);
	*/

	//Create a vehicle that will drive on the plane.
	VehicleDesc vehicleDesc = initVehicleDesc();
	gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);
	PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	//Create Box
	//PxRigidDynamic* box1 = NULL; //actor

	PxTransform t = PxTransform(PxVec3(0, 20.0f, 0.0f));
	PxShape* boxShape = gPhysics->createShape(PxBoxGeometry(1, 1, 1), *gMaterial);
	//PxTransform localTm(PxVec3(PxReal(1 * 2) - PxReal(2 - 1), PxReal(1 * 2 + 1), 0) * 0.5);
	box1 = gPhysics->createRigidDynamic(t);
	PxFilterData boxFilterData(COLLISION_FLAG_GROUND_AGAINST,COLLISION_FLAG_OBSTACLE , 0, 0);
	boxShape->setSimulationFilterData(boxFilterData);
	box1->attachShape(*boxShape);
	PxRigidBodyExt::updateMassAndInertia(*box1, 10.0f);
	gScene->addActor(*box1);

	PxTransform t2 = PxTransform(PxVec3(0.0f, 0.0f, 6.0f));
	PxShape* boxShape2 = gPhysics->createShape(PxBoxGeometry(1.0f, 2.0f, 0.5f), *gMaterial);
	//PxTransform localTm(PxVec3(PxReal(1 * 2) - PxReal(2 - 1), PxReal(1 * 2 + 1), 0) * 0.5);
	box2 = gPhysics->createRigidStatic(t2);
	PxFilterData boxFilterData2(COLLISION_FLAG_GROUND_AGAINST, COLLISION_FLAG_OBSTACLE, 0, 0);
	boxShape2->setSimulationFilterData(boxFilterData2);
	box2->attachShape(*boxShape2);
	//PxRigidBodyExt::setMassAndUpdateInertia(*box2, 10.0f);
	gScene->addActor(*box2);

	PxTransform t3 = PxTransform(PxVec3(6.0f, 1.0f, 6.0f), PxQuat(0.785398f, PxVec3(0.f, 1.f, 0.f)));
	PxShape* boxShape3 = gPhysics->createShape(PxBoxGeometry(1.0f, 1.0f, 1.0f), *gMaterial);
	//PxTransform localTm(PxVec3(PxReal(1 * 2) - PxReal(2 - 1), PxReal(1 * 2 + 1), 0) * 0.5);
	box3 = gPhysics->createRigidStatic(t3);
	//PxFilterData boxFilterData3(COLLISION_FLAG_GROUND_AGAINST, COLLISION_FLAG_OBSTACLE, 0, 0);
	boxShape3->setSimulationFilterData(boxFilterData2);
	box3->attachShape(*boxShape3);
	//PxRigidBodyExt::setMassAndUpdateInertia(*box2, 10.0f);
	gScene->addActor(*box3);


	boxShape->release();
	boxShape2->release();
	boxShape3->release();

	//Create Mesh Object
	PxVec3 convexVerts[] = {PxVec3(0,1,0),PxVec3(1,0,0),PxVec3(-1,0,0),PxVec3(0,0,1),
	PxVec3(0,0,-1) };
	auto mesh = scene->addEntity();
	mesh->getComponent<PhysicsComponent>()->addActor();

	

	/*
	PxTransform mesht = PxTransform(PxVec3(0, 20.0f, 0.0f));
	PxF32 x = 3.0f;
	PxF32 y = 3.0f;
	PxF32 z = 3.0f;
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
	
	PxConvexMesh* mesh = createConvexMesh(verts, 8, *gPhysics, *gCooking);
	
	PxFilterData meshQryFilterData;
	setupNonDrivableSurface(meshQryFilterData);
	meshShape->setQueryFilterData(meshQryFilterData);
	meshShape->setLocalPose(PxTransform(PxIdentity));
	//PxTransform localTm(PxVec3(PxReal(1 * 2) - PxReal(2 - 1), PxReal(1 * 2 + 1), 0) * 0.5);
	mesh1 = gPhysics->createRigidDynamic(mesht);
	PxFilterData meshFilterData(COLLISION_FLAG_GROUND_AGAINST, COLLISION_FLAG_OBSTACLE, 0, 0);
	meshShape->setSimulationFilterData(meshFilterData);
	//PxShape* chassisShape = PxRigidActorExt::createExclusiveShape(*vehActor, PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]);
	mesh1->attachShape(*meshShape);
	PxRigidBodyExt::updateMassAndInertia(*mesh1, 10.0f);
	gScene->addActor(*mesh1);

	
	*/

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);

	gVehicleModeTimer = 0.0f;
	gVehicleOrderProgress = 0;
	vehicleBrakeMode(0.0f);
}

void PhysicsSystem::cleanupPhysics()
{
	gVehicle4W->getRigidDynamicActor()->release();
	gVehicle4W->free();
	PX_RELEASE(gGroundPlane);
	PX_RELEASE(gBatchQuery);
	gVehicleSceneQueryData->free(gAllocator);
	PX_RELEASE(gFrictionPairs);
	PxCloseVehicleSDK();

	PX_RELEASE(gMaterial);
	PX_RELEASE(gCooking);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);

	printf("SnippetVehicle4W done.\n");
}

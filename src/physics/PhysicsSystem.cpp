#include "PhysicsSystem.h"
#include "Model.h"

#include <sstream>
#include <fstream>
#include <string>

PhysicsSystem::PhysicsSystem(
    std::shared_ptr<Scene> scene)
    : gameScene(scene)
{
    /* PhysX initialization */
    initPhysX();

	// Register physics component initialization events
	Events::RigidbodyComponentInit.registerHandler<
		PhysicsSystem, &PhysicsSystem::registerRigidbodyComponent>(this);

	Events::VehicleComponentInit.registerHandler<
		PhysicsSystem, &PhysicsSystem::registerVehicleComponent>(this);

	// Register vehicle handler events
	Events::VehicleAccelerate.registerHandler<
		PhysicsSystem, &PhysicsSystem::vehicleAccelerateMode>(this);

	Events::VehicleSteer.registerHandler<
		PhysicsSystem, &PhysicsSystem::vehicleTurnMode>(this);

	Events::VehicleBrake.registerHandler<
		PhysicsSystem, &PhysicsSystem::vehicleBrakeMode>(this);

	Events::VehicleHandbrake.registerHandler<
		PhysicsSystem, &PhysicsSystem::vehicleHandbrakeMode>(this);
}

/* PhysX per-frame updates */
void PhysicsSystem::update() {
    for (auto it = gameScene->begin(); it != gameScene->end(); it++) {
		// Update each vehicle given it's input values
		if (auto vc = it->getComponent<VehicleComponent>()) {
			vehicleUpdate(vc);
		}
	}

    simulateScene();

	// Retrieve array of actors that moved
	PxU32 nbActiveActors;
	PxActor** activeActors = pxScene->getActiveActors(nbActiveActors);

	// Update each render object with the new transform
	for (PxU32 i = 0; i < nbActiveActors; i++) {
		if (PxRigidActor* actor = activeActors[i]->is<PxRigidActor>()) {
			Entity* entity = static_cast<Entity*>(activeActors[i]->userData);
			PxTransform pxTransform = actor->getGlobalPose();
			if (auto tc = entity->getComponent<TransformComponent>()) {
				tc->setLocalPosition(pxTransform.p);
				tc->setLocalRotation(pxTransform.q);
			}
		}
	}

}


void PhysicsSystem::vehicleUpdate(shared_ptr<VehicleComponent> vc) {

	// Get reference to vehicle
	PxVehicleDrive4W* vehicle = vc->vehicle;

	// Smooth inputs
	PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(
		gPadSmoothingData, 
		gSteerVsForwardSpeedTable, 
		vc->inputData, 
		Time::timeStep(), 
		vc->isInAir, 
		*vehicle
	);


	// When reversing while car is moving forwards, brake instead
	PxReal speed = vehicle->computeForwardSpeed();
	PxReal input = vehicle->mDriveDynData.getAnalogInput(
		PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL);

	if (speed > 0.0 && input < 0.0) 
		vc->inputData.setAnalogBrake(-input);
	else
		vc->inputData.setAnalogBrake(0.0f);


	// Raycasts
	PxVehicleWheels* vehicles[1] = { vehicle };
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);


	// Vehicle update
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];

	PxVehicleWheelQueryResult vehicleQueryResults[1] = {{
		wheelQueryResults, 
		vehicle->mWheelsSimData.getNbWheels()
	}};

	PxVehicleUpdates(
		Time::timeStep(), 
		pxScene->getGravity(), 
		*gFrictionPairs, 
		1, 
		vehicles, 
		vehicleQueryResults
	);


	// Work out if the vehicle is in the air
	vc->isInAir = vehicle->getRigidDynamicActor()->isSleeping() ?
		false : PxVehicleIsInAir(vehicleQueryResults[0]);

}

void PhysicsSystem::simulateScene()
{
	// Scene update
	pxScene->simulate(Time::timeStep());
	pxScene->fetchResults(true);
}


void PhysicsSystem::vehicleAccelerateMode(Entity& entity, float v) 
{
	auto vc = entity.getComponent<VehicleComponent>();
	if (!vc) return;

	vc->inputData.setAnalogBrake(0.0f);
	
	if (v >= 0.0f) {
		vc->vehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	}
	else if (v < 0.0f) {
		vc->vehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
		v *= -1;
	}

	vc->inputData.setAnalogAccel(v);
}

void PhysicsSystem::vehicleTurnMode(Entity& entity, float v) 
{
	auto vc = entity.getComponent<VehicleComponent>();
	if (!vc) return;

	vc->inputData.setAnalogSteer(v);
}

void PhysicsSystem::vehicleBrakeMode(Entity& entity, float v) 
{
	auto vc = entity.getComponent<VehicleComponent>();
	if (!vc) return;

	vc->inputData.setAnalogBrake(v);
}

void PhysicsSystem::vehicleHandbrakeMode(Entity& entity, float v) 
{
	auto vc = entity.getComponent<VehicleComponent>();
	if (!vc) return;

	vc->inputData.setAnalogHandbrake(v);
}


void PhysicsSystem::initPhysX()
{

	/* Foundation, PVD, physics, and cooking */
	// -------------------------------------

	pxFoundation = PxCreateFoundation(
		PX_PHYSICS_VERSION, 
		pxAllocator, 
		pxErrorCallback
	);
	
	pxPvd = PxCreatePvd(*pxFoundation);
	pxPvd->connect(
		*PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10),
		PxPvdInstrumentationFlag::eALL
	);

	pxPhysics = PxCreatePhysics(
		PX_PHYSICS_VERSION, 
		*pxFoundation, 
		PxTolerancesScale(), 
		true, 					 // Track outstanding allocations
		pxPvd
	);

	pxCooking = PxCreateCooking(
		PX_PHYSICS_VERSION, 
		*pxFoundation, 
		PxCookingParams(PxTolerancesScale())
	);


	/* Create scene and other stuff */
	// ----------------------------

	PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;

	PxU32 numWorkers = 1;
	pxDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = pxDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;

	// Create physics scene
	pxScene = pxPhysics->createScene(sceneDesc);
	PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS,    true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES,true);
	}


	/* Prepare Vehicle SDK */
	// -------------------

	// A material ???
	gMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxInitVehicleSDK(*pxPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(
		1, PX_MAX_NB_WHEELS, 1, 1, 
		WheelSceneQueryPreFilterBlocking, 
		NULL, pxAllocator
	);

	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, pxScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);


	//Create a plane to drive on (Should probably be moved out to higher level)
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	pxGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, pxPhysics);
	pxScene->addActor(*pxGroundPlane);

	

}

void PhysicsSystem::cleanupPhysics()
{
	
	PX_RELEASE(pxGroundPlane);

	PX_RELEASE(gBatchQuery);
	gVehicleSceneQueryData->free(pxAllocator);
	PX_RELEASE(gFrictionPairs);
	PxCloseVehicleSDK();

	PX_RELEASE(gMaterial);
	PX_RELEASE(pxCooking);
	PX_RELEASE(pxScene);
	PX_RELEASE(pxDispatcher);
	PX_RELEASE(pxPhysics);
	
	if (pxPvd) {
		PxPvdTransport* transport = pxPvd->getTransport();
		pxPvd->release();	
		pxPvd = NULL;
		PX_RELEASE(transport);
	}

	PX_RELEASE(pxFoundation);
	
}



/* Called whenever a rigidbody component is created */
void PhysicsSystem::registerRigidbodyComponent(RigidbodyComponent& component) {
	component.setPhysicsSystem(
		dynamic_pointer_cast<PhysicsSystem>(shared_from_this()));
}

/* Called whenever a vehicle component is created */
void PhysicsSystem::registerVehicleComponent(VehicleComponent& component) {
	component.setPhysicsSystem(
		dynamic_pointer_cast<PhysicsSystem>(shared_from_this()));
}


/** Convert Physx::PxVec3 to glm::vec3
 @param[in] vec3 The Physx::PxVec3
 @param[ou] Converted glm::mat4
 */
void PhysicsSystem::PhysXVec3ToglmVec3(const PxVec3& vec3, glm::vec3& newVec)
{
	newVec = glm::vec3(-vec3.x, vec3.y, vec3.z);
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

PhysicsSystem::~PhysicsSystem() {}



/** KAYNENS CODE FOR HEIGHTMAPS
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
	PxHeightField* aHeightField = cooking->createHeightField(hfDesc, physics->getPhysicsInsertionCallback());
	//The row and column scales tell the system how far apart the sampled points lie in the associated direction
	//The height scale scales the integer height values to a floating point range
	PxHeightFieldGeometry hfGeom(aHeightField, PxMeshGeometryFlags(), heightScale, rowScale, colScale);
	//The variant of createExclusiveShape() used here specifies an array of materials for the height field, which will be indexed by
	//the material indices of each cell to resolve collisions with that cell. The single-material variant may be used instead, but
	//the height field material indices must all be a single value or the special value eHOLE.
	// Actor might be a PxRigidStatic*??
	// PxRigidActor& actor, const PxGeometry& geometry, PxMaterial*const* materials, PxU16 materialCount,PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE)
	PxTransform pose(PxVec3(-((PxReal)numRows * rowScale) / 2.0f, 0.0f, -((PxReal)numCols * colScale) / 2.0f), PxQuat(PxIdentity));
	PxRigidActor* hf = physics->createRigidStatic(pose);
	PxShape* aHeightFieldShape = PxRigidActorExt::createExclusiveShape(*hf, hfGeom, *gMaterial);
	PxFilterData qryFilterData2;
	setupDrivableSurface(qryFilterData2);
	aHeightFieldShape->setQueryFilterData(qryFilterData2);
	*/

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
	
	Events::VolumeTriggerComponentInit.registerHandler<
		PhysicsSystem, &PhysicsSystem::registerVolumeTriggerComponent>(this);

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

	Events::VehicleFlip.registerHandler<
		PhysicsSystem, &PhysicsSystem::vehicleFlipMode>(this);
}

/* PhysX per-frame updates */
void PhysicsSystem::update() {
	// Update each vehicle given it's input values
	for (auto& wp_vc : gameScene->iterate<VehicleComponent>()) {
		vehicleUpdate(wp_vc);
	}

    simulateScene();

	// Retrieve array of actors that moved
	PxU32 nbActiveActors;
	PxActor** activeActors = pxScene->getActiveActors(nbActiveActors);

	// Update each render object with the new transform
	for (PxU32 i = 0; i < nbActiveActors; i++) {

		// Retrieve actor's rigidbody representation if possible
		PxRigidActor* actor = activeActors[i]->is<PxRigidActor>();
		if (!actor) continue; // If no rigidbody, go to next actor

		// Retrieve the entity associated with the actor
		Entity* entity = static_cast<Entity*>(actor->userData);
		if (!entity) continue; // If no associated entity, go to next actor

		try {
			auto sp_entity = entity->shared_from_this();
		} catch (std::exception) {
			printf("actor userdata invalid @ location (%.1f, %.1f)\n", actor->getGlobalPose().p.x, actor->getGlobalPose().p.z);
			continue;
		}

		// If entity has a transform component, set position/orientation
		if (auto tc = entity->getComponent<TransformComponent>()) {
			// Get the actor's transform
			PxTransform pxTransform = actor->getGlobalPose();
			tc->setLocalPosition(pxTransform.p);
			tc->setLocalRotation(pxTransform.q);
			
			// If entity is a vehicle, set wheel positions
			if (auto vc = entity->getComponent<VehicleComponent>()) {
				for (int i = 0; i < 4; i++) {
					if (!vc->wheelEntities[i]) continue;
					
					PxShape* wheelShape = vc->wheelShapes[i];
					PxTransform wheelTransform = wheelShape->getLocalPose();
					
					// For left wheels
					if (wheelTransform.p.x < 0.0f) {
						PxQuat rotate180 = PxQuat(glm::radians(180.0f), PxVec3(0.0f, 1.0f, 0.0f));
						wheelTransform.q = rotate180 * wheelTransform.q;
					}

					auto gameTrans = vc->wheelEntities[i]->getComponent<TransformComponent>();
					gameTrans->setLocalPosition(wheelTransform.p);
					gameTrans->setLocalRotation(wheelTransform.q);
				}
			}
		}

	}

}

PxRigidActor* PhysicsSystem::createTriggerBox(PxTransform startPos, PxBoxGeometry boxGeom) {
	// test triggerShape
	PxRigidActor* trigger = pxPhysics->createRigidStatic(startPos);
	PxShape* triggerShape = PxRigidActorExt::createExclusiveShape(*trigger, boxGeom, *gMaterial);
	//PxFilterFlags Tflags;

	triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	pxScene->addActor(*trigger);
	triggerShape->release();
	return trigger;

}

/*
* 
dims.x = radius
dims.y = half height

*/
PxRigidActor* PhysicsSystem::createCylinderStatic(PxTransform startPos, PxVec3 dims) {
	// test triggerShape
	PxRigidActor* cylActor = pxPhysics->createRigidStatic(startPos);
	PxShape* cylShape = PxRigidActorExt::createExclusiveShape(*cylActor,PxCapsuleGeometry(dims.x, dims.y), *gMaterial);
	//PxFilterFlags Tflags;

	cylShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	cylShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	pxScene->addActor(*cylActor);
	cylShape->release();
	return cylActor;
}



PxTriangleMesh* PhysicsSystem::createStaticMesh(const Model& model) {

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

	pxCooking->setParams(params);
	PxTriangleMeshDesc meshDesc;
	vector<Mesh> arrMeshes = model.getMeshes();
	
	meshDesc.points.count = arrMeshes[0].vertsPos.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = arrMeshes[0].vertsPos.data();

	meshDesc.triangles.count = arrMeshes[0].indicies.size()/3.0f;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = arrMeshes[0].indicies.data();

#ifdef _DEBUG
	// mesh should be validated before cooked without the mesh cleaning
	bool res = pxCooking->validateTriangleMesh(meshDesc);
	PX_ASSERT(res);
#endif

	PxTriangleMesh* triMesh = pxCooking->createTriangleMesh(meshDesc,
		pxPhysics->getPhysicsInsertionCallback());

	return triMesh;
}


PxConvexMesh* PhysicsSystem::createDynamicMesh(const Model& model) {

    // Create descriptor for convex mesh
	PxConvexMeshDesc convexDesc;
	vector<Mesh> arrMeshes = model.getMeshes();

	convexDesc.points.count = arrMeshes[0].vertsPos.size();
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = arrMeshes[0].vertsPos.data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	convexDesc.indices.count = arrMeshes[0].indicies.size();
	convexDesc.indices.stride = 3 * sizeof(PxU32);
	convexDesc.indices.data = arrMeshes[0].indicies.data();
	
	// Initialize mesh to null
	PxConvexMesh* convexMesh = nullptr;

	// Attempt to create mesh
	PxDefaultMemoryOutputStream buf;
	if (pxCooking->cookConvexMesh(convexDesc, buf))
    {
        PxDefaultMemoryInputData inputData(buf.getData(), buf.getSize());
        convexMesh = pxPhysics->createConvexMesh(inputData);
    }
	
	// Return result
	return convexMesh;
}

PxConvexMesh* PhysicsSystem::createDynamicMesh(const PxVec3* verts, const PxU32 numVerts)
{
    // Create descriptor for convex mesh
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = numVerts;
    convexDesc.points.stride = sizeof(PxVec3);
    convexDesc.points.data = verts;
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

    // Initialize mesh to null
	PxConvexMesh* convexMesh = nullptr;

	// Attempt to create mesh
	PxDefaultMemoryOutputStream buf;
	if (pxCooking->cookConvexMesh(convexDesc, buf))
    {
        PxDefaultMemoryInputData inputData(buf.getData(), buf.getSize());
        convexMesh = pxPhysics->createConvexMesh(inputData);
    }
	
	// Return result
	return convexMesh;
}


void PhysicsSystem::vehicleUpdate(weak_ptr<VehicleComponent> wp_vc) {

	// Get reference to vehicle
	auto vc = wp_vc.lock(); if (!vc) return;
	PxVehicleDrive4W* vehicle = vc->vehicle;

	// Smooth inputs
	PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(
		gPadSmoothingData, 
		gSteerVsForwardSpeedTable, 
		vc->inputData, 
		Time::timeStep(), 
		vc->isInAir_, 
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
	// PxVehicleWheels* vehicles[1] = { vehicle };
	// PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	// const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	// PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Suspension sweeps (instead of raycasts).
	//Sweeps provide more information about the geometry under the wheel.
	PxVehicleWheels* vehicles[1] = { vehicle };
	PxSweepQueryResult* sweepResults = gVehicleSceneQueryData->getSweepQueryResultBuffer(0);
	const PxU32 sweepResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionSweeps(gBatchQuery, 1, vehicles, sweepResultsSize, sweepResults, 3, NULL, 1.0f, 1.01f);


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
	vc->isInAir_ = vehicle->getRigidDynamicActor()->isSleeping() ?
		false : PxVehicleIsInAir(vehicleQueryResults[0]);

	

}

void PhysicsSystem::simulateScene()
{
	// Scene update
	pxScene->simulate(Time::timeStep());
	pxScene->fetchResults(true);
}


void PhysicsSystem::vehicleAccelerateMode(weak_ptr<Entity> wpEntity, float v) 
{

	// Get shared ptr to entity
	auto entity = wpEntity.lock();
	if (!entity) return;

	// Get vehicle component of entity
	auto vc = entity->getComponent<VehicleComponent>();
	if (!vc) return;
	if (!vc->isDisabled()) {
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
}

void PhysicsSystem::vehicleTurnMode(weak_ptr<Entity> wpEntity, float v) 
{
	// Get shared ptr to entity
	auto entity = wpEntity.lock();
	if (!entity) return;

	auto vc = entity->getComponent<VehicleComponent>();
	if (!vc) return;
	if (!vc->isDisabled()) {
		vc->inputData.setAnalogSteer(-v);
	}
}

void PhysicsSystem::vehicleBrakeMode(weak_ptr<Entity> wpEntity, float v) 
{
	// Get shared ptr to entity
	auto entity = wpEntity.lock();
	if (!entity) return;

	auto vc = entity->getComponent<VehicleComponent>();
	if (!vc) return;
	if (!vc->isDisabled()) {
		vc->inputData.setAnalogBrake(v);
	}
}

void PhysicsSystem::vehicleHandbrakeMode(weak_ptr<Entity> wpEntity, float v) 
{
	// Get shared ptr to entity
	auto entity = wpEntity.lock();
	if (!entity) return;

	auto vc = entity->getComponent<VehicleComponent>();
	if (!vc) return;
	if (!vc->isDisabled()) {
		vc->inputData.setAnalogHandbrake(v);
	}
}

void PhysicsSystem::vehicleFlipMode(weak_ptr<Entity> wpEntity, float v)
{
	// Get shared ptr to entity
	auto entity = wpEntity.lock();
	if (!entity) return;
	auto vc = entity->getComponent<VehicleComponent>();
	if (!vc) return;
	if (!vc->isDisabled()) {
		PxTransform trans = vc->vehicle->getRigidDynamicActor()->getGlobalPose();
		// Disallow flipping if car too high or spinning too fast
		if (trans.p.y > 2.75f || 
		vc->vehicle->getRigidDynamicActor()->getAngularVelocity().magnitudeSquared() > 10.0f) {
			return;
		}
		// Increase jump if wheels are on the ground (full 360)
		float jumpForce = 3.0f;
		if (vc->isGrounded()) jumpForce = 6.0f;
		vc->vehicle->getRigidDynamicActor()->addForce(PxVec3(0.0f, jumpForce, 0.0f), PxForceMode::eVELOCITY_CHANGE);
		vc->vehicle->getRigidDynamicActor()->addTorque(trans.q.rotate(PxVec3(-0.5f,0.0f, 1.5f)), PxForceMode::eVELOCITY_CHANGE);
	}
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

	pxSimEventCallback = make_shared<PhysXSimCallback>();
	pxScene->setSimulationEventCallback(pxSimEventCallback.get());


	/* Prepare Vehicle SDK */
	// -------------------

	// A material ???
	gMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxInitVehicleSDK(*pxPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	PxVehicleSetSweepHitRejectionAngles(0.9f, 1.5f);

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

/* Called whenever a vehicle component is created */
void PhysicsSystem::registerVolumeTriggerComponent(VolumeTriggerComponent& component) {
	component.setPhysicsSystem(
		dynamic_pointer_cast<PhysicsSystem>(shared_from_this()));
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

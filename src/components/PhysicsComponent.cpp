#include "PhysicsComponent.h"
#include "PhysicsSystem.h"
#include "Model.h"


PhysicsComponent::PhysicsComponent(Entity& parent) 
    : BaseComponent(parent)
{}



void PhysicsComponent::addActorStatic(Model model, PxTransform startPos){

	PxTransform mesht = startPos;
	PxRigidActor* mesh1 = gPhysics->createRigidStatic(mesht);

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

	gCooking->setParams(params);
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
	bool res = gCooking->validateTriangleMesh(meshDesc);
	PX_ASSERT(res);
#endif
	PxTriangleMesh* triMesh = gCooking->createTriangleMesh(meshDesc,
		gPhysics->getPhysicsInsertionCallback());
	PxTriangleMeshGeometry geom(triMesh);
	PxShape* meshShape = PxRigidActorExt::createExclusiveShape(*mesh1, geom, *gMaterial);
	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	meshShape->setSimulationFilterData(meshFilterData);
	gScene->addActor(*mesh1);
	meshShape->release();
}

void PhysicsComponent::addActorDynamic(Model model, PxTransform startPos) {

	PxTransform mesht = startPos;//PxTransform(PxVec3(0, 20.0f, 0.0f));
	PxRigidDynamic* mesh1 = gPhysics->createRigidDynamic(mesht);
	PxConvexMeshDesc convexDesc;
	vector<Mesh> arrMeshes = model.getMeshes();

	convexDesc.points.count = arrMeshes[0].vertsPos.size();
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = arrMeshes[0].vertsPos.data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	convexDesc.indices.count = arrMeshes[0].indicies.size();
	convexDesc.indices.stride = 3 * sizeof(PxU32);
	convexDesc.indices.data = arrMeshes[0].indicies.data();
	
	PxDefaultMemoryOutputStream buf;
	if (!gCooking->cookConvexMesh(convexDesc, buf)) {
		return;
	};

	PxDefaultMemoryInputData readBuffer(buf.getData(), buf.getSize());
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);
	PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*mesh1,PxConvexMeshGeometry(convexMesh),*gMaterial);
	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	aConvexShape->setSimulationFilterData(meshFilterData);
	gScene->addActor(*mesh1);
	aConvexShape->release();
}

ComponentEnum PhysicsComponent::getType() {
    return ComponentEnum::physics;
}

PhysicsComponent::~PhysicsComponent() {
    // Nothing to do here yet
}

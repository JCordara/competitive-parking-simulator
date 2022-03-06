#include "RigidbodyComponent.h"
#include "physics/PhysicsSystem.h"


RigidbodyComponent::RigidbodyComponent(Entity& parent) 
    : BaseComponent(parent) 
{
	// Notify Physics system that this component was created
    // This sets the reference to the physics system
	Events::RigidbodyComponentInit.broadcast(*this);
}


void RigidbodyComponent::addActorStatic(Model model, PxTransform startPos){

	PxTransform mesht = startPos;
	PxRigidActor* mesh1 = physicsSystem->pxPhysics->createRigidStatic(mesht);

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

	physicsSystem->pxCooking->setParams(params);
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
	bool res = physicsSystem->pxCooking->validateTriangleMesh(meshDesc);
	PX_ASSERT(res);
#endif

	PxTriangleMesh* triMesh = physicsSystem->pxCooking->createTriangleMesh(meshDesc,
		physicsSystem->pxPhysics->getPhysicsInsertionCallback());
	PxTriangleMeshGeometry geom(triMesh);
	PxShape* meshShape = PxRigidActorExt::createExclusiveShape(*mesh1, geom, *gMaterial);
	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	meshShape->setSimulationFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*mesh1);
	meshShape->release();
}

void RigidbodyComponent::addActorDynamic(Model model, PxTransform startPos) {

	PxTransform mesht = startPos;//PxTransform(PxVec3(0, 20.0f, 0.0f));
	PxRigidDynamic* mesh1 = physicsSystem->pxPhysics->createRigidDynamic(mesht);
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
	if (!physicsSystem->pxCooking->cookConvexMesh(convexDesc, buf)) {
		return;
	};

	PxDefaultMemoryInputData readBuffer(buf.getData(), buf.getSize());
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = physicsSystem->pxPhysics->createConvexMesh(input);
	PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*mesh1,PxConvexMeshGeometry(convexMesh),*gMaterial);
	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	aConvexShape->setSimulationFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*mesh1);
	aConvexShape->release();
}

ComponentEnum RigidbodyComponent::getType() {
    return ComponentEnum::rigidbody;
}

RigidbodyComponent::~RigidbodyComponent() {
    // Nothing to do here yet
}

#include "RigidbodyComponent.h"
#include "Physics/PhysicsSystem.h"


RigidbodyComponent::RigidbodyComponent(shared_ptr<Entity> parent) 
    : BaseComponent(parent) 
{
	// Notify Physics system that this component was created
    // This sets the reference to the physics system
	Events::RigidbodyComponentInit.broadcast(*this);
}


void RigidbodyComponent::addActorStaticMesh(const Model& model, PxTransform startPos){

	PxTransform mesht = startPos;
	actor = physicsSystem->pxPhysics->createRigidStatic(mesht);
	actor->userData = static_cast<void*>(entity.get());

	PxTriangleMesh* mesh = physicsSystem->createStaticMesh(model);
	PxTriangleMeshGeometry geom(mesh);

	PxShape* meshShape = PxRigidActorExt::createExclusiveShape(*actor, geom, *gMaterial);

	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	meshShape->setSimulationFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*actor);
	meshShape->release();
}

void RigidbodyComponent::addActorStaticSphere(const float radius, PxTransform startPos) {

	actor = physicsSystem->pxPhysics->createRigidStatic(startPos);
	actor->userData = static_cast<void*>(entity.get());

	PxSphereGeometry sphereGeom(radius);

	PxShape* sphere = PxRigidActorExt::createExclusiveShape(*actor, sphereGeom, *gMaterial);

	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	sphere->setSimulationFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*actor);
	sphere->release();
}


void RigidbodyComponent::addActorStaticBox(PxVec3 halfLen, PxTransform startPos) {

	actor = physicsSystem->pxPhysics->createRigidStatic(startPos);
	actor->userData = static_cast<void*>(entity.get());

	PxBoxGeometry boxGeom(halfLen);

	PxShape* box = PxRigidActorExt::createExclusiveShape(*actor, boxGeom, *gMaterial);

	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	box->setSimulationFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*actor);
	box->release();
}
void RigidbodyComponent::addActorDynamic(const Model& model, PxTransform startPos) {

	PxTransform mesht = startPos;//PxTransform(PxVec3(0, 20.0f, 0.0f));
	actor = physicsSystem->pxPhysics->createRigidDynamic(mesht);
	actor->userData = static_cast<void*>(entity.get());

	PxConvexMesh* mesh = physicsSystem->createDynamicMesh(model);
	PxConvexMeshGeometry geom(mesh);

	PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*actor, geom, *gMaterial);
	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	aConvexShape->setSimulationFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*actor);
	//actor->setMass(50.0f);
	aConvexShape->release();
}


ComponentEnum RigidbodyComponent::getType() {
    return ComponentEnum::rigidbody;
}

RigidbodyComponent::~RigidbodyComponent() {
    physicsSystem->pxScene->removeActor(*actor, true);
}

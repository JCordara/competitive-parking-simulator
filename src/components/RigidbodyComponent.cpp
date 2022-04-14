#include "RigidbodyComponent.h"
#include "Physics/PhysicsSystem.h"


RigidbodyComponent::RigidbodyComponent(weak_ptr<Entity> parent)
    : BaseComponent(parent) 
{
	// Notify Physics system that this component was created
    // This sets the reference to the physics system
	Events::RigidbodyComponentInit.broadcast(*this);
}


void RigidbodyComponent::addActorStaticMesh(const Model& model, PxTransform startPos, float sFriction, float dFriction){

	PxTransform mesht = startPos;
	actor = physicsSystem->pxPhysics->createRigidStatic(mesht);
	actor->userData = static_cast<void*>(getEntity().get());

	PxTriangleMesh* mesh = physicsSystem->createStaticMesh(model);
	PxTriangleMeshGeometry geom(mesh);

	PxMaterial* material = physicsSystem->pxPhysics->createMaterial(sFriction, dFriction, 0.05f);
	PxShape* meshShape = PxRigidActorExt::createExclusiveShape(*actor, geom, *material);

	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	setupDrivableSurface(meshFilterData);
	meshShape->setSimulationFilterData(meshFilterData);
	meshShape->setQueryFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*actor);
	meshShape->release();
}

void RigidbodyComponent::addActorStaticCapsule(const float halfHeight, const float radius, PxTransform startPos, float sFriction, float dFriction) {

	actor = physicsSystem->pxPhysics->createRigidStatic(startPos);
	actor->userData = static_cast<void*>(getEntity().get());

	PxCapsuleGeometry capsuleGeom(radius, halfHeight);

	PxMaterial* material = physicsSystem->pxPhysics->createMaterial(sFriction, dFriction, 0.05f);
	PxShape* capsule = PxRigidActorExt::createExclusiveShape(*actor, capsuleGeom, *material);

	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	setupDrivableSurface(meshFilterData);
	capsule->setSimulationFilterData(meshFilterData);
	capsule->setQueryFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*actor);
	capsule->release();
}


void RigidbodyComponent::addActorStaticBox(PxVec3 halfLen, PxTransform startPos) {

	actor = physicsSystem->pxPhysics->createRigidStatic(startPos);
	actor->userData = static_cast<void*>(getEntity().get());

	PxBoxGeometry boxGeom(halfLen);

	PxShape* box = PxRigidActorExt::createExclusiveShape(*actor, boxGeom, *gMaterial);

	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	setupDrivableSurface(meshFilterData);
	box->setSimulationFilterData(meshFilterData);
	box->setQueryFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*actor);
	box->release();
}

void RigidbodyComponent::addActorDynamic(const Model& model, PxTransform startPos) {

	PxTransform mesht = startPos;//PxTransform(PxVec3(0, 20.0f, 0.0f));
	actor = physicsSystem->pxPhysics->createRigidDynamic(mesht);
	actor->userData = static_cast<void*>(getEntity().get());

	PxConvexMesh* mesh = physicsSystem->createDynamicMesh(model);
	PxConvexMeshGeometry geom(mesh);

	PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*actor, geom, *gMaterial);
	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	setupDrivableSurface(meshFilterData);
	aConvexShape->setSimulationFilterData(meshFilterData);
	aConvexShape->setQueryFilterData(meshFilterData);
	
	PxRigidBodyExt::updateMassAndInertia(*actor->is<PxRigidBody>(), 875.0f);
	physicsSystem->pxScene->addActor(*actor);
	aConvexShape->release();
}


ComponentEnum RigidbodyComponent::getType() {
    return ComponentEnum::rigidbody;
}

RigidbodyComponent::~RigidbodyComponent() {
    physicsSystem->pxScene->removeActor(*actor, true);
}

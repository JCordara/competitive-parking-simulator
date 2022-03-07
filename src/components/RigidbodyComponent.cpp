#include "RigidbodyComponent.h"
#include "physics/PhysicsSystem.h"


RigidbodyComponent::RigidbodyComponent(Entity& parent) 
    : BaseComponent(parent) 
{
	// Notify Physics system that this component was created
    // This sets the reference to the physics system
	Events::RigidbodyComponentInit.broadcast(*this);
}


void RigidbodyComponent::addActorStatic(const Model& model, PxTransform startPos){

	PxTransform mesht = startPos;
	PxRigidActor* actor = physicsSystem->pxPhysics->createRigidStatic(mesht);
	actor->userData = &entity;

	PxTriangleMeshGeometry geom = physicsSystem->createStaticMesh(model);
	PxShape* meshShape = PxRigidActorExt::createExclusiveShape(*actor, geom, *gMaterial);

	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	meshShape->setSimulationFilterData(meshFilterData);

	physicsSystem->pxScene->addActor(*actor);
	meshShape->release();
}

void RigidbodyComponent::addActorDynamic(const Model& model, PxTransform startPos) {

	PxTransform mesht = startPos;//PxTransform(PxVec3(0, 20.0f, 0.0f));
	PxRigidDynamic* actor = physicsSystem->pxPhysics->createRigidDynamic(mesht);
	actor->userData = &entity;

	PxConvexMeshGeometry geom = physicsSystem->createDynamicMesh(model);

	PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*actor, geom, *gMaterial);
	PxFilterData meshFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	aConvexShape->setSimulationFilterData(meshFilterData);
	physicsSystem->pxScene->addActor(*actor);
	aConvexShape->release();
}


ComponentEnum RigidbodyComponent::getType() {
    return ComponentEnum::rigidbody;
}

RigidbodyComponent::~RigidbodyComponent() {
    // Nothing to do here yet
}

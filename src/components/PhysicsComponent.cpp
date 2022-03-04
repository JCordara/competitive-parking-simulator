#include "PhysicsComponent.h"

#include "SnippetVehicle4W.h"

PhysicsComponent::PhysicsComponent(Entity& parent) 
    : BaseComponent(parent)
{}



void PhysicsComponent::addActor(){

	PxTransform mesht = PxTransform(PxVec3(0, 20.0f, 0.0f));
	PxRigidDynamic* mesh1 = gPhysics->createRigidDynamic(mesht);
	static const PxVec3 convexVerts[] = {PxVec3(0,1,0),PxVec3(1,0,0),PxVec3(-1,0,0),PxVec3(0,0,1),
	PxVec3(0,0,-1) };
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = sizeof(convexVerts);
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = convexVerts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;
	if (!gCooking->cookConvexMesh(convexDesc, buf, &result)) {
		return;
	};

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);
	PxShape* meshShape = PxRigidActorExt::createExclusiveShape(*mesh1, PxConvexMeshGeometry(convexMesh), *gMaterial);
	PxFilterData meshFilterData(COLLISION_FLAG_GROUND_AGAINST, COLLISION_FLAG_OBSTACLE, 0, 0);
	meshShape->setSimulationFilterData(meshFilterData);
	gScene->addActor(*mesh1);
	meshShape->release();
}

ComponentEnum PhysicsComponent::getType() {
    return ComponentEnum::physics;
}

PhysicsComponent::~PhysicsComponent() {
    // Nothing to do here yet
}

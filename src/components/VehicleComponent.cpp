#include "VehicleComponent.h"

#include "physics/PhysicsSystem.h"
#include "physics/VehicleDesc.h"


VehicleComponent::VehicleComponent(Entity& parent) 
    : BaseComponent(parent)
{

    // Notify Physics system that this component was created
    // This sets the reference to the physics system
    Events::VehicleComponentInit.broadcast(*this);

	// Get position of parent entity
	PxTransform entityPose = PxTransform(PxIdentity);

	if (auto tc = entity.getComponent<TransformComponent>()) {
		entityPose.p.x = tc->getLocalPosition().x;
		entityPose.p.y = tc->getLocalPosition().y;
		entityPose.p.z = tc->getLocalPosition().z;
		entityPose.q   = tc->getLocalRotation();
	}

    // Create a vehicle
	VehicleDesc vehicleDesc = initVehicleDesc();
	vehicle = createVehicle4W(
        vehicleDesc, 
        physicsSystem->pxPhysics, 
        physicsSystem->pxCooking,
		entity
    );

    // Add vehicle to physics scene
	vehicle->getRigidDynamicActor()->setGlobalPose(entityPose);
	physicsSystem->pxScene->addActor(*vehicle->getRigidDynamicActor());
}

VehicleDesc VehicleComponent::initVehicleDesc()
{
	// Set up the chassis mass, dimensions, moment of inertia, and center of mass offset
	// The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering
	// Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
	const PxVec3 chassisMOI(
		(chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
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
	vehicleDesc.wheelSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

ComponentEnum VehicleComponent::getType() {
    return ComponentEnum::vehicle;
}

VehicleComponent::~VehicleComponent() {
    // Nothing to do here yet
}

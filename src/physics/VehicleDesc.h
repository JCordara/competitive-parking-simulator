#ifndef VEHICLE_DESC_H
#define VEHICLE_DESC_H

#include <PhysX/PxPhysicsAPI.h>

using namespace physx;


struct ActorUserData
{
    ActorUserData()
        : vehicle(NULL),
        actor(NULL)
    {
    }

    const PxVehicleWheels* vehicle;
    const PxActor* actor;
};

struct ShapeUserData
{
    ShapeUserData()
        : isWheel(false),
        wheelId(0xffffffff)
    {
    }

    bool isWheel;
    PxU32 wheelId;
};

struct VehicleDesc
{
    VehicleDesc()
        : chassisMass(0.0f),
        chassisDims(PxVec3(0.0f, 0.0f, 0.0f)),
        chassisMOI(PxVec3(0.0f, 0.0f, 0.0f)),
        chassisCMOffset(PxVec3(0.0f, 0.0f, 0.0f)),
        chassisMaterial(NULL),
        wheelMass(0.0f),
        wheelWidth(0.0f),
        wheelRadius(0.0f),
        wheelMOI(0.0f),
        wheelMaterial(NULL),
        actorUserData(NULL),
        shapeUserDatas(NULL)
    {
    }

    PxF32 chassisMass;
    PxVec3 chassisDims;
    PxVec3 chassisMOI;
    PxVec3 chassisCMOffset;
    PxMaterial* chassisMaterial;
    PxFilterData chassisSimFilterData;  //word0 = collide type, word1 = collide against types, word2 = PxPairFlags

    PxF32 wheelMass;
    PxF32 wheelWidth;
    PxF32 wheelRadius;
    PxF32 wheelMOI;
    PxMaterial* wheelMaterial;
    PxU32 numWheels;
    PxFilterData wheelSimFilterData;	//word0 = collide type, word1 = collide against types, word2 = PxPairFlags

    ActorUserData* actorUserData;
    ShapeUserData* shapeUserDatas;
};

#endif // VEHICLE_DESC_H

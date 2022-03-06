#ifndef VEHICLE_COMPONENT_H
#define VEHICLE_COMPONENT_H

#include "Components.h"
#include <PhysX/PxPhysicsAPI.h>

class PhysicsSystem;
class VehicleDesc;

using namespace physx;

class VehicleComponent : public BaseComponent {
public:

    friend class PhysicsSystem;

    VehicleComponent(Entity& parent);
    ~VehicleComponent();
    static ComponentEnum getType();

    inline void setPhysicsSystem(shared_ptr<PhysicsSystem> system) {
        physicsSystem = system;
    }

private:

    VehicleDesc initVehicleDesc();

    shared_ptr<PhysicsSystem> physicsSystem;

    PxVehicleDrive4W* vehicle;
    PxVehicleDrive4WRawInputData inputData;
    bool isInAir;

};

#endif // VEHICLE_COMPONENT_H

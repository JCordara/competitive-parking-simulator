#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "GameSystem.h"

#include "PhysX/PxPhysicsAPI.h"
#include "SnippetVehicle4W.h"


using namespace physx;

class PhysicsSystem : public GameSystem {
public:

    /* Prepare framework */
    PhysicsSystem(
        std::shared_ptr<Scene> scene,
        std::shared_ptr<AudioManager> audio
    );

    void update();
    void PhysXMat4ToglmMat4(const PxMat44& mat4, glm::mat4& newMat);
	void PhysXVec3ToglmVec3(const PxVec3& vec3, glm::vec3& newVec);
    ~PhysicsSystem();
    
private:

    std::shared_ptr<Scene>             scene;
	std::shared_ptr<AudioManager>      audioManager;

    VehicleDesc initVehicleDesc();
    void initPhysics();
    void stepPhysics();
    void cleanupPhysics();

};


#endif // PHYSICS_SYSTEM_H

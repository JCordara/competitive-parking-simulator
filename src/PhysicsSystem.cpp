#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<GameEventManager> event,
    std::shared_ptr<AudioManager> audio)
    : scene(scene)
    , eventManager(event)
    , audioManager(audio)
{
    /* PhysX initialization */
    initPhysics();
	gVehicleInputData.setDigitalBrake(false);
}

void PhysicsSystem::update() {
    /* PhysX per-frame updates */
    stepPhysics();
}

/** Convert PhysX::PxMat44 to glm::mat4
 @param[in] mat4 The PhysX::PxMat44
 @param[ou] Converted glm::mat4
 */
void PhysicsSystem::PhysXMat4ToglmMat4(const PxMat44& mat4, glm::mat4& newMat)
{

	newMat[0][0] = mat4[0][0];
	newMat[0][1] = mat4[0][1];
	newMat[0][2] = mat4[0][2];
	newMat[0][3] = mat4[0][3];

	newMat[1][0] = mat4[1][0];
	newMat[1][1] = mat4[1][1];
	newMat[1][2] = mat4[1][2];
	newMat[1][3] = mat4[1][3];

	newMat[2][0] = mat4[2][0];
	newMat[2][1] = mat4[2][1];
	newMat[2][2] = mat4[2][2];
	newMat[2][3] = mat4[2][3];

	newMat[3][0] = mat4[3][0];
	newMat[3][1] = mat4[3][1];
	newMat[3][2] = mat4[3][2];
	newMat[3][3] = mat4[3][3];

}

/** Convert Physx::PxVec3 to glm::vec3
 @param[in] vec3 The Physx::PxVec3
 @param[ou] Converted glm::mat4
 */
void PhysicsSystem::PhysXVec3ToglmVec3(const PxVec3& vec3, glm::vec3& newVec)
{
	newVec = glm::vec3(vec3.x, vec3.y, vec3.z);
}

PhysicsSystem::~PhysicsSystem() {
    
}
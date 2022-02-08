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
}

void PhysicsSystem::update() {
    /* PhysX per-frame updates */
    Log::debug("Physics update!");
}

PhysicsSystem::~PhysicsSystem() {
    
}

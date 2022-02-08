#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<GameEventManager> event,
    std::shared_ptr<AudioManager> audio)
    : scene(scene)
    , eventManager(event)
    , audioManager(audio) 
{}

void PhysicsSystem::update() {
    Log::debug("Physics update!");
}

PhysicsSystem::~PhysicsSystem() {
    
}

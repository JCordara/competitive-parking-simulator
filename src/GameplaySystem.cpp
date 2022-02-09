#include "GameplaySystem.h"

GameplaySystem::GameplaySystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<GameEventManager> event,
    std::shared_ptr<AudioManager> audio)
    : scene(scene)
    , eventManager(event)
    , audioManager(audio) 
{}
    
void GameplaySystem::update() {

}

GameplaySystem::~GameplaySystem() {

}

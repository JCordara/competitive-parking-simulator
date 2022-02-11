#include "GameplaySystem.h"

GameplaySystem::GameplaySystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<GameEventManager> event,
    std::shared_ptr<AudioManager> audio,
    GameObject& c)
    : scene(scene)
    , eventManager(event)
    , audioManager(audio) 
    , car(c)
{}
    
void GameplaySystem::update() {
    for (auto e : scene->entities()) {
        auto c = e->getComponent<VolumeTriggerComponent>();
        if (c) {
            c->checkForEntity(car);
        }
    }
}

GameplaySystem::~GameplaySystem() {

}
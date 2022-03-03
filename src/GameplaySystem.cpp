#include "GameplaySystem.h"

GameplaySystem::GameplaySystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<AudioManager> audio,
    GameObject& c)
    : scene(scene)
    , audioManager(audio) 
    , car(c)
{}
    
void GameplaySystem::update() {
    for (auto it = scene->begin(); it != scene->end(); it++) {
        auto vc = it->getComponent<VolumeTriggerComponent>();
        if (vc) {
            vc->checkForEntity(car);
        }
        /*
        auto ac = e->getComponent<AiComponent>();
        if (ac) {
            ac->update();
        }
        */
    }
}

GameplaySystem::~GameplaySystem() {

}

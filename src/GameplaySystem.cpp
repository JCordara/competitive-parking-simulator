#include "GameplaySystem.h"

GameplaySystem::GameplaySystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<AudioManager> audio)
    : scene(scene)
    , audioManager(audio) 
{}
    
void GameplaySystem::update() {
    /*
    for (auto it = scene->begin(); it != scene->end(); it++) {
        auto ac = e->getComponent<AiComponent>();
        if (ac) {
            ac->update();
        }
    }
    */
}

GameplaySystem::~GameplaySystem() {

}

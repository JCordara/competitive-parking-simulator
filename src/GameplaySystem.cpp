#include "GameplaySystem.h"

GameplaySystem::GameplaySystem(std::shared_ptr<Scene> scene)
    : scene(scene)
{}
    
void GameplaySystem::update() {
	/*for (auto it = scene->begin(); it != scene->end(); it++) {
		auto volumeTriggerC = it->getComponent<VolumeTriggerComponent>();
		if (volumeTriggerC) {
			//ac->update();
		}
	}




    for (auto it = scene->begin(); it != scene->end(); it++) {
        auto ac = it->getComponent<AiComponent>();
        if (ac) {
            //ac->update();
        }
    }*/
}

GameplaySystem::~GameplaySystem() {

}

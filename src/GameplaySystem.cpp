#include "GameplaySystem.h"

GameplaySystem::GameplaySystem(std::shared_ptr<Scene> scene)
    : scene(scene)
{
    Events::AiComponentInit.registerHandler<GameplaySystem,
        &GameplaySystem::registerAiComponent>(this);
}
    
void GameplaySystem::update() {

	for (auto it = scene->begin(); it != scene->end(); it++) {
		auto volumeTriggerC = it->getComponent<VolumeTriggerComponent>();
		if (volumeTriggerC) {
			// volumeTriggerC->update();
		}
	}

    for (auto it = scene->begin(); it != scene->end(); it++) {
        auto ac = it->getComponent<AiComponent>();
        if (ac) {
            // ac->update();
        }
    }
}

GameplaySystem::~GameplaySystem() {

}

void GameplaySystem::registerAiComponent(AiComponent& component) {
    component.setGameplaySystem(
        dynamic_pointer_cast<GameplaySystem>(shared_from_this()));
}


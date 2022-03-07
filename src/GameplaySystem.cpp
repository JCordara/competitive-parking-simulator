#include "GameplaySystem.h"

GameplaySystem::GameplaySystem(std::shared_ptr<Scene> scene)
    : scene(scene)
{
    Events::AiComponentInit.registerHandler<GameplaySystem,
        &GameplaySystem::registerAiComponent>(this);

	setupAiNodes();
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

void GameplaySystem::setupAiNodes() {

	// Spawn Node Collection
	std::shared_ptr<AiGraphNode> aiNode1 = std::make_shared<AiGraphNode>();
	aiNode1->nodeType = AiGraphNode::NodeType::SPAWN;
	aiNode1->position = glm::vec3();
	aiGlobalNodes.push_back(aiNode1);
	//

	// Outer Roads Node Collection

	//

	// Entrances Node Collection

	//

	// Inner Parking Lot Node Collection

	//

	// Open Parking Stall Node Colelction

	//

	// Intersection Node Collection
}

void GameplaySystem::registerAiComponent(AiComponent& component) {
	component.setGameplaySystem(
		dynamic_pointer_cast<GameplaySystem>(shared_from_this()));
}

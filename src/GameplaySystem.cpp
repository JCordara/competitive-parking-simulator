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
            ac->update();
        }
    }
}

GameplaySystem::~GameplaySystem() {

}

void GameplaySystem::setupAiNodes() {

	// Spawn Node Collection
	std::shared_ptr<AiGraphNode> aiNode1 = std::make_shared<AiGraphNode>();
	aiNode1->nodeType = AiGraphNode::NodeType::SPAWN;
	aiNode1->position = glm::vec3(-6,1,2.5);
	aiGlobalNodes.push_back(aiNode1);
	//

	// Outer Roads Node Collection
	std::shared_ptr<AiGraphNode> aiNode2 = std::make_shared<AiGraphNode>();
	aiNode2->nodeType = AiGraphNode::NodeType::OUTERROAD;
	aiNode2->position = glm::vec3(1.5, 1, 13);
	aiGlobalNodes.push_back(aiNode2);

	std::shared_ptr<AiGraphNode> aiNode3 = std::make_shared<AiGraphNode>();
	aiNode3->nodeType = AiGraphNode::NodeType::OUTERROAD;
	aiNode3->position = glm::vec3(1.5, 1, -8);
	aiGlobalNodes.push_back(aiNode3);

	//

	// Entrances Node Collection
	std::shared_ptr<AiGraphNode> aiNode4 = std::make_shared<AiGraphNode>();
	aiNode4->nodeType = AiGraphNode::NodeType::LOTENTRANCE;
	aiNode4->position = glm::vec3(-19, 1, 35);
	aiGlobalNodes.push_back(aiNode4);

	std::shared_ptr<AiGraphNode> aiNode5 = std::make_shared<AiGraphNode>();
	aiNode5->nodeType = AiGraphNode::NodeType::LOTENTRANCE;
	aiNode5->position = glm::vec3(23, 1, -32);
	aiGlobalNodes.push_back(aiNode5);
	//

	// Inner Parking Lot Node Collection
	std::shared_ptr<AiGraphNode> aiNode6 = std::make_shared<AiGraphNode>();
	aiNode6->nodeType = AiGraphNode::NodeType::INNERLOT;
	aiNode6->position = glm::vec3(-38, 1, 35);
	aiGlobalNodes.push_back(aiNode6);

	std::shared_ptr<AiGraphNode> aiNode7 = std::make_shared<AiGraphNode>();
	aiNode7->nodeType = AiGraphNode::NodeType::INNERLOT;
	aiNode7->position = glm::vec3(43, 1, -32);
	aiGlobalNodes.push_back(aiNode7);
	//

	// Open Parking Stall Node Colelction
	std::shared_ptr<AiGraphNode> aiNode8 = std::make_shared<AiGraphNode>();
	aiNode8->nodeType = AiGraphNode::NodeType::PARKINGSTALL;
	aiNode8->position = glm::vec3(-45, 1, 53);
	aiGlobalNodes.push_back(aiNode8);

	std::shared_ptr<AiGraphNode> aiNode9 = std::make_shared<AiGraphNode>();
	aiNode9->nodeType = AiGraphNode::NodeType::PARKINGSTALL;
	aiNode9->position = glm::vec3(43, 1, -53);
	aiGlobalNodes.push_back(aiNode9);
	//

	// Intersection Node Collection
	std::shared_ptr<AiGraphNode> aiNode10 = std::make_shared<AiGraphNode>();
	aiNode10->nodeType = AiGraphNode::NodeType::INTERSECTION;
	aiNode10->position = glm::vec3(-1.5, 1, 35);
	aiGlobalNodes.push_back(aiNode10);

	std::shared_ptr<AiGraphNode> aiNode11 = std::make_shared<AiGraphNode>();
	aiNode11->nodeType = AiGraphNode::NodeType::INTERSECTION;
	aiNode11->position = glm::vec3(1.5, 1, -32);
	aiGlobalNodes.push_back(aiNode11);

	std::shared_ptr<AiGraphNode> aiNode12 = std::make_shared<AiGraphNode>();
	aiNode12->nodeType = AiGraphNode::NodeType::INTERSECTION;
	aiNode12->position = glm::vec3(1.5, 1, 69);
	aiGlobalNodes.push_back(aiNode12);
	//
}

void GameplaySystem::registerAiComponent(AiComponent& component) {
	component.setGameplaySystem(
		dynamic_pointer_cast<GameplaySystem>(shared_from_this()));
}

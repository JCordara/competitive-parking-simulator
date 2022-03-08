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

	
	std::shared_ptr<AiGraphNode> aiNode1 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode2 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode3 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode4 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode5 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode6 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode7 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode8 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode9 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode10 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode11 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode12 = std::make_shared<AiGraphNode>();

	// Spawn Node Collection
	aiNode1->nodeType = AiGraphNode::NodeType::SPAWN;
	aiNode1->id = 1;
	aiNode1->position = glm::vec3(-6,1,2.5);
	aiNode1->neighbours.push_back(aiNode2);
	aiNode1->neighbours.push_back(aiNode3);
	aiGlobalNodes.push_back(aiNode1);
	//

	// Outer Roads Node Collection
	aiNode2->nodeType = AiGraphNode::NodeType::OUTERROAD;
	aiNode2->id = 2;
	aiNode2->position = glm::vec3(1.5, 1, 13);
	aiNode2->neighbours.push_back(aiNode1);
	aiNode2->neighbours.push_back(aiNode10);
	aiGlobalNodes.push_back(aiNode2);

	aiNode3->nodeType = AiGraphNode::NodeType::OUTERROAD;
	aiNode3->id = 3;
	aiNode3->position = glm::vec3(1.5, 1, -8);
	aiNode3->neighbours.push_back(aiNode1);
	aiNode3->neighbours.push_back(aiNode11);
	aiGlobalNodes.push_back(aiNode3);
	//

	// Entrances Node Collection
	aiNode4->nodeType = AiGraphNode::NodeType::LOTENTRANCE;
	aiNode4->id = 4;
	aiNode4->position = glm::vec3(-19, 1, 41);
	aiNode4->neighbours.push_back(aiNode10);
	aiNode4->neighbours.push_back(aiNode6);
	aiGlobalNodes.push_back(aiNode4);
	
	aiNode5->nodeType = AiGraphNode::NodeType::LOTENTRANCE;
	aiNode5->id = 5;
	aiNode5->position = glm::vec3(23, 1, -34);
	aiNode5->neighbours.push_back(aiNode11);
	aiNode5->neighbours.push_back(aiNode7);
	aiGlobalNodes.push_back(aiNode5);
	//

	// Inner Parking Lot Node Collection
	aiNode6->nodeType = AiGraphNode::NodeType::INNERLOT;
	aiNode6->id = 7;
	aiNode6->position = glm::vec3(-38, 1, 35);
	aiNode6->neighbours.push_back(aiNode4);
	aiNode6->neighbours.push_back(aiNode8);
	aiGlobalNodes.push_back(aiNode6);
	
	aiNode7->nodeType = AiGraphNode::NodeType::INNERLOT;
	aiNode7->id = 7;
	aiNode7->position = glm::vec3(43, 1, -33);
	aiNode7->neighbours.push_back(aiNode5);
	aiNode7->neighbours.push_back(aiNode9);
	aiGlobalNodes.push_back(aiNode7);
	//

	// Open Parking Stall Node Colelction
	aiNode8->nodeType = AiGraphNode::NodeType::PARKINGSTALL;
	aiNode8->id = 8;
	aiNode8->position = glm::vec3(-45, 1, 60);
	aiNode8->neighbours.push_back(aiNode6);
	aiGlobalNodes.push_back(aiNode8);
	
	aiNode9->nodeType = AiGraphNode::NodeType::PARKINGSTALL;
	aiNode9->id = 9;
	aiNode9->position = glm::vec3(47, 1, -55);
	aiNode9->neighbours.push_back(aiNode7);
	aiGlobalNodes.push_back(aiNode9);
	//

	// Intersection Node Collection
	aiNode10->nodeType = AiGraphNode::NodeType::INTERSECTION;
	aiNode10->id = 10;
	aiNode10->position = glm::vec3(1.5, 1, 37);
	aiNode10->neighbours.push_back(aiNode2);
	aiNode10->neighbours.push_back(aiNode12);
	aiNode10->neighbours.push_back(aiNode4);
	aiGlobalNodes.push_back(aiNode10);
	
	aiNode11->nodeType = AiGraphNode::NodeType::INTERSECTION;
	aiNode11->id = 11;
	aiNode11->position = glm::vec3(1.5, 1, -32);
	aiNode11->neighbours.push_back(aiNode3);
	aiNode11->neighbours.push_back(aiNode5);
	aiGlobalNodes.push_back(aiNode11);
	
	aiNode12->nodeType = AiGraphNode::NodeType::INTERSECTION;
	aiNode12->id = 12;
	aiNode12->position = glm::vec3(1.5, 1, 69);
	aiNode12->neighbours.push_back(aiNode10);
	aiGlobalNodes.push_back(aiNode12);
	//
}

void GameplaySystem::registerAiComponent(AiComponent& component) {
	component.setGameplaySystem(
		dynamic_pointer_cast<GameplaySystem>(shared_from_this()));
}

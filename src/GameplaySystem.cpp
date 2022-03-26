#include "GameplaySystem.h"

GameplaySystem::GameplaySystem(std::shared_ptr<Scene> scene)
    : scene(scene)
{
    Events::AiComponentInit.registerHandler<GameplaySystem,
        &GameplaySystem::registerAiComponent>(this);

	Events::CarParked.registerHandler<GameplaySystem,
		&GameplaySystem::registerCarParked>(this);


	setupAiNodes();
}
    
void GameplaySystem::update() {
	for (auto& ai : scene->iterate<AiComponent>()) {
		ai->update();
    }
}

GameplaySystem::~GameplaySystem() {

}

void GameplaySystem::registerAiComponent(AiComponent& component) {
	component.setGameplaySystem(
		dynamic_pointer_cast<GameplaySystem>(shared_from_this()));
}


void GameplaySystem::registerCarParked(shared_ptr<Entity> entity) {
	std::cout << entity->id() << " , " << playerId << endl;
	if (entity->id() == playerId) {
		
		if (scene->getEntityByID(playerId)->getComponent<VehicleComponent>()->getSpeed() <= 3.0f) {
			scores[playerId]++;
			resetPlayer();
		};
		
	}
	// Must be AI
	else {
		unordered_map<unsigned int, int>::iterator it;
		for (it = scores.begin(); it != scores.end(); it++)
		{
			if (it->first == entity->id()) {
				scores[it->first]++;
				//resetAi(it->first);
				entity->getComponent<AiComponent>()->resetAi();
			}
		}
	}
}

void GameplaySystem::setPlayerId(unsigned int playerId) {
	playerId = playerId;
	scores[playerId] = 0;
}

void GameplaySystem::addAiId(unsigned int aiId) {
	aiList.push_back(aiId);
	scores[aiId] = 0;
}

void GameplaySystem::resetPlayer() {
	scene->getEntityByID(playerId)->
	getComponent<TransformComponent>()->
	setLocalPosition(glm::vec3(0, 0.1, 0));
	scene->getEntityByID(playerId)->
		getComponent<TransformComponent>()->
		setLocalRotation(Random::randomFloat(0.0f,6.28f), glm::vec3(0.0f, 1.0f, 0.0f));
	scene->getEntityByID(playerId)->
		getComponent<VehicleComponent>()->vehicle->getRigidDynamicActor()->setLinearVelocity(PxVec3(0.0f));
}

bool GameplaySystem::gameWon(){
	if (scores[playerId] >= 5 || scores[aiList[0]] >= 5) {
		if (scores[playerId] >= 5) {
			return true;
		}

		if (scores[aiList[0]] >= 5) {
			return true;
		}
	}
	else {
		return false;
	}
	return true;
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
	std::shared_ptr<AiGraphNode> aiNode13 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode14 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode15 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode16 = std::make_shared<AiGraphNode>();
	std::shared_ptr<AiGraphNode> aiNode17 = std::make_shared<AiGraphNode>();

	// Spawn Node Collection
	aiNode1->nodeType = AiGraphNode::NodeType::SPAWN;
	aiNode1->id = 1;
	aiNode1->position = glm::vec3(-10, 1, -7);
	aiNode1->neighbours.push_back(aiNode2);
	aiNode1->neighbours.push_back(aiNode3);
	aiGlobalNodes.push_back(aiNode1);

	aiNode15->nodeType = AiGraphNode::NodeType::SPAWN;
	aiNode15->id = 15;
	aiNode15->position = glm::vec3(-10, 1, 6);
	aiNode15->neighbours.push_back(aiNode2);
	aiNode15->neighbours.push_back(aiNode3);
	aiGlobalNodes.push_back(aiNode15);

	aiNode16->nodeType = AiGraphNode::NodeType::SPAWN;
	aiNode16->id = 16;
	aiNode16->position = glm::vec3(10, 1, -7);
	aiNode16->neighbours.push_back(aiNode2);
	aiNode16->neighbours.push_back(aiNode3);
	aiGlobalNodes.push_back(aiNode16);

	aiNode17->nodeType = AiGraphNode::NodeType::SPAWN;
	aiNode17->id = 17;
	aiNode17->position = glm::vec3(10, 1, 8);
	aiNode17->neighbours.push_back(aiNode2);
	aiNode17->neighbours.push_back(aiNode3);
	aiGlobalNodes.push_back(aiNode17);
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
	aiNode6->id = 6;
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

	aiNode14->nodeType = AiGraphNode::NodeType::INNERLOT;
	aiNode14->id = 14;
	aiNode14->position = glm::vec3(135, 1, -23.2);
	aiNode14->neighbours.push_back(aiNode13);
	aiNode14->neighbours.push_back(aiNode7);
	aiGlobalNodes.push_back(aiNode14);
	//

	// Open Parking Stall Node Colelction
	aiNode8->nodeType = AiGraphNode::NodeType::PARKINGSTALL;
	aiNode8->id = 8;
	aiNode8->position = glm::vec3(-45.2, 1, 61);
	aiNode8->neighbours.push_back(aiNode6);
	aiGlobalNodes.push_back(aiNode8);

	aiNode9->nodeType = AiGraphNode::NodeType::PARKINGSTALL;
	aiNode9->id = 9;
	aiNode9->position = glm::vec3(46.4, 1, -57);
	aiNode9->neighbours.push_back(aiNode7);
	aiGlobalNodes.push_back(aiNode9);

	aiNode13->nodeType = AiGraphNode::NodeType::PARKINGSTALL;
	aiNode13->id = 13;
	aiNode13->position = glm::vec3(148.5, 1, -23.2);
	aiNode13->neighbours.push_back(aiNode14);
	aiGlobalNodes.push_back(aiNode13);
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

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

void GameplaySystem::setNodeType(std::string nodeType, std::shared_ptr<AiGraphNode> aiNode) {
	// spawn entrance traversal parking spot
	if (!nodeType.compare("spawn")) {
		aiNode->nodeType = AiGraphNode::NodeType::SPAWN;
	}
	if (!nodeType.compare("entrance")) {
		aiNode->nodeType = AiGraphNode::NodeType::LOTENTRANCE;
	}
	if (!nodeType.compare("traversal")) {
		aiNode->nodeType = AiGraphNode::NodeType::TRAVERSAL;
	}
	if (!nodeType.compare("parking")) {
		aiNode->nodeType = AiGraphNode::NodeType::PARKINGSTALL;
	}
}

void GameplaySystem::addAINode(std::string nodeType, int nodeAreaCode, glm::vec3 nodePos) {
	std::shared_ptr<AiGraphNode> aiNode = std::make_shared<AiGraphNode>();
	setNodeType(nodeType, aiNode);
	lastNodeID++; aiNode->id = lastNodeID;
	aiNode->position = nodePos;
	switch (nodeAreaCode) {
	case 950: area950Nodes.push_back(aiNode); aiNode->areaCode = 950; break;
	case 951: area951Nodes.push_back(aiNode); aiNode->areaCode = 951; break;
	case 952: area952Nodes.push_back(aiNode); aiNode->areaCode = 952; break;
	case 953: area953Nodes.push_back(aiNode); aiNode->areaCode = 953; break;
	case 954: area954Nodes.push_back(aiNode); aiNode->areaCode = 954; break;
	case 955: area955Nodes.push_back(aiNode); aiNode->areaCode = 955; break;
	case 956: area956Nodes.push_back(aiNode); aiNode->areaCode = 956; break;
	case 957: area957Nodes.push_back(aiNode); aiNode->areaCode = 957; break;
	case 958: area958Nodes.push_back(aiNode); aiNode->areaCode = 958; break;
	case 959: area959Nodes.push_back(aiNode); aiNode->areaCode = 959; break;
	case 960: area960Nodes.push_back(aiNode); aiNode->areaCode = 960; break;
	case 961: area961Nodes.push_back(aiNode); aiNode->areaCode = 961; break;
	case 962: area962Nodes.push_back(aiNode); aiNode->areaCode = 962; break;
	case 963: area963Nodes.push_back(aiNode); aiNode->areaCode = 963; break;
	case 964: area964Nodes.push_back(aiNode); aiNode->areaCode = 964; break;
	case 965: area965Nodes.push_back(aiNode); aiNode->areaCode = 965; break;
	case 966: area966Nodes.push_back(aiNode); aiNode->areaCode = 966; break;
	case 967: area967Nodes.push_back(aiNode); aiNode->areaCode = 967; break;
	case 968: area968Nodes.push_back(aiNode); aiNode->areaCode = 968; break;
	case 969: area969Nodes.push_back(aiNode); aiNode->areaCode = 969; break;
	case 970: area970Nodes.push_back(aiNode); aiNode->areaCode = 970; break;
	default:
		break;
	}
	aiGlobalNodes.push_back(aiNode);
	
}

void GameplaySystem::testPrintAINodes() {
	//std::cout << "-----AREA 970-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area970Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		if (node->id == 109) { // Middle Traversal
			setAINodeNeighbors(node, 59, area970Nodes); // right entrance
			setAINodeNeighbors(node, 60, area970Nodes); // Bottom entrance
			setAINodeNeighbors(node, 61, area970Nodes); // Top entrance
			setAINodeNeighbors(node, 62, area970Nodes); // Left entrance
		}
		else if (node->id == 59) setAINodeNeighbors(node, 8, area952Nodes);
		else if (node->id == 60) setAINodeNeighbors(node, 17, area954Nodes);
		else if (node->id == 61) setAINodeNeighbors(node, 4, area950Nodes);
		else if (node->id == 62) setAINodeNeighbors(node, 24, area956Nodes);
		else {
			// All spawn nodes are the same
			setAINodeNeighbors(node, 59, area970Nodes);
			setAINodeNeighbors(node, 60, area970Nodes);
			setAINodeNeighbors(node, 61, area970Nodes);
			setAINodeNeighbors(node, 62, area970Nodes);
			setAINodeNeighbors(node, 109, area970Nodes);
		}

	}
	//std::cout << "-----AREA 950-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area950Nodes) {
		//std::cout	<< "Node Type: " << static_cast<int>(node->nodeType)
		//			<< " Position: " << node->position
		//			<<" ID: " << node->id << std::endl;
		//1=Left-Ent,2=Top-Ent,3=Right-Ent,4=Bottom-Ent,115=Top-Tra,116=Bottom-Tra
		switch (node->id){
		case 1: setAINodeNeighbors(node, 30, area958Nodes); break;
		case 2: setAINodeNeighbors(node, 54, area968Nodes); break;
		case 3: setAINodeNeighbors(node, 7, area951Nodes); break;
		case 116: setAINodeNeighbors(node, 3, area950Nodes);
			setAINodeNeighbors(node, 4, area950Nodes); break;
			setAINodeNeighbors(node, 115, area950Nodes); break; //Push for both
		case 115: setAINodeNeighbors(node, 1, area950Nodes);
			setAINodeNeighbors(node, 2, area950Nodes); break;
		}
	}
	//std::cout << "-----AREA 952-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area952Nodes) {
			//std::cout	<< "Node Type: " << static_cast<int>(node->nodeType)
			//			<< " Position: " << node->position
			//			<<" ID: " << node->id << std::endl;
		//8=Left-Ent,9=BottomLeft-Ent,10=Top-Ent,11=BottomRight-Ent,12=Right-Ent
		//110=LeftMiddle-Tra,111=Left-Tra,112=RightMiddle-Tra,113=Right-Tra,114=Middle-Tra,
		//121=Left-Park,122=Right-Park
		switch (node->id) {
		case 8: setAINodeNeighbors(node, 111, area952Nodes); break;
		case 9: setAINodeNeighbors(node, 15, area953Nodes); break;
		case 10: setAINodeNeighbors(node, 5, area951Nodes); break;
		case 11: setAINodeNeighbors(node, 13, area953Nodes); break;
		case 12: setAINodeNeighbors(node, 116, area952Nodes); break;
		case 110: setAINodeNeighbors(node, 114, area952Nodes); break;
		case 111: setAINodeNeighbors(node, 8, area952Nodes);
			setAINodeNeighbors(node, 9, area952Nodes); break;
			setAINodeNeighbors(node, 110, area952Nodes); break;
		case 112: setAINodeNeighbors(node, 10, area952Nodes);
			setAINodeNeighbors(node, 113, area952Nodes); break;
		case 113: setAINodeNeighbors(node, 12, area952Nodes);
			setAINodeNeighbors(node, 11, area952Nodes);
			setAINodeNeighbors(node, 122, area952Nodes); break;
		case 114: setAINodeNeighbors(node, 112, area952Nodes);
			setAINodeNeighbors(node, 10, area952Nodes);
			setAINodeNeighbors(node, 121, area952Nodes); break;
		}
	}
	//std::cout << "-----AREA 954-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area954Nodes) {
			//std::cout	<< "Node Type: " << static_cast<int>(node->nodeType)
			//			<< " Position: " << node->position
			//			<<" ID: " << node->id << std::endl;
		//17=Top-Ent,18=Right-Ent,19=Bottom-Ent,20=Left-Ent,72=Middle-Tra
		switch (node->id) {
		case 17: setAINodeNeighbors(node, 20, area954Nodes);
			setAINodeNeighbors(node, 18, area954Nodes); break;
		case 18: setAINodeNeighbors(node, 19, area954Nodes);
			setAINodeNeighbors(node, 16, area953Nodes); break;
		case 19: setAINodeNeighbors(node, 20, area954Nodes);
			setAINodeNeighbors(node, 43, area966Nodes); break;
		case 20: setAINodeNeighbors(node, 21, area955Nodes); break;
		case 72: setAINodeNeighbors(node, 17, area954Nodes);
			setAINodeNeighbors(node, 18, area954Nodes);
			setAINodeNeighbors(node, 19, area954Nodes);
			setAINodeNeighbors(node, 20, area954Nodes); break;
		}
	}
	//std::cout << "-----AREA 956-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area956Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//24=Right-Ent,23=Bottom-Ent,25=Top-Ent,26=Left-Ent,74=Right-Tra,75=Left-Tra
		switch (node->id) {
		case 24: setAINodeNeighbors(node, 23, area954Nodes);
			setAINodeNeighbors(node, 25, area954Nodes); break;
		case 23: setAINodeNeighbors(node, 22, area955Nodes); break;
		case 25: setAINodeNeighbors(node, 28, area958Nodes); break;
		case 26: setAINodeNeighbors(node, 31, area959Nodes); break;
		case 74: setAINodeNeighbors(node, 75, area954Nodes);
			setAINodeNeighbors(node, 24, area954Nodes);
			setAINodeNeighbors(node, 23, area954Nodes);
			setAINodeNeighbors(node, 25, area954Nodes); break;
		case 75:setAINodeNeighbors(node, 26, area954Nodes);
			setAINodeNeighbors(node, 25, area954Nodes); break;
		}
	}
	//std::cout << "-----AREA 951-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area951Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//7=Left-Ent,5=Bottom-Ent,6=Top-Ent,63=Left-Tra,64=LeftMiddle-Tra,65=RightMiddle-Tra,66=Right-Tra
		//120=Left-Park,119=LeftMiddle-park,117=RightMiddle-Park,118=Right-Park
		switch (node->id) {
		case 6: setAINodeNeighbors(node, 53, area968Nodes); break;
		case 63: setAINodeNeighbors(node, 7, area951Nodes);
			setAINodeNeighbors(node, 64, area951Nodes);
			setAINodeNeighbors(node, 120, area951Nodes); break;
		case 64: setAINodeNeighbors(node, 65, area951Nodes);
			setAINodeNeighbors(node, 119, area951Nodes);
			setAINodeNeighbors(node, 120, area951Nodes); break;
		case 65: setAINodeNeighbors(node, 66, area951Nodes);
			setAINodeNeighbors(node, 6, area951Nodes);
			setAINodeNeighbors(node, 5, area951Nodes);
			setAINodeNeighbors(node, 117, area951Nodes); break;
		case 66: setAINodeNeighbors(node, 6, area951Nodes);
			setAINodeNeighbors(node, 5, area951Nodes);
			setAINodeNeighbors(node, 118, area951Nodes); break;
		}
	}
	//std::cout << "-----AREA 953-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area953Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//15=TopLeft-Ent,13=TopRight-Ent,16=Left-Ent,14=Bottom-Ent
		//67=Left-Tra,68=LeftMiddle-Tra,69=Middle-Tra,70=RightMioddle-Tra,71=Right-Tra
		//123=Left-Park,124=Right-Park
		switch (node->id) {
		case 14: setAINodeNeighbors(node, 124, area953Nodes);
			setAINodeNeighbors(node, 45, area966Nodes); break;
		case 67: setAINodeNeighbors(node, 68, area953Nodes);
			setAINodeNeighbors(node, 15, area953Nodes);
			setAINodeNeighbors(node, 16, area953Nodes); break;
		case 68: setAINodeNeighbors(node, 69, area953Nodes); break;
		case 69: setAINodeNeighbors(node, 70, area953Nodes);
			setAINodeNeighbors(node, 123, area953Nodes); break;
		case 70: setAINodeNeighbors(node, 71, area953Nodes);
			setAINodeNeighbors(node, 14, area953Nodes);
			setAINodeNeighbors(node, 124, area953Nodes); break;
		case 71: setAINodeNeighbors(node, 13, area953Nodes);
			setAINodeNeighbors(node, 14, area953Nodes);
			setAINodeNeighbors(node, 124, area953Nodes); break;
		}
	}
	//std::cout << "-----AREA 955-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area955Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//21=Right-Ent,22=Top-Ent
		switch (node->id) {
		case 21: setAINodeNeighbors(node, 125, area955Nodes); break;
		case 22: setAINodeNeighbors(node, 125, area955Nodes); break;
		case 73: setAINodeNeighbors(node, 21, area955Nodes);
			setAINodeNeighbors(node, 22, area955Nodes);
			setAINodeNeighbors(node, 125, area955Nodes); break;
		}
	}
	//std::cout << "-----AREA 958-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area958Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//30=Right-Ent,28=Bottom-Ent,29=Left-Ent,77=Bottom-Tra,78=Top-Tra
		switch (node->id) {
		case 29: setAINodeNeighbors(node, 27, area957Nodes); break;
		case 77: setAINodeNeighbors(node, 78, area958Nodes);
			setAINodeNeighbors(node, 29, area958Nodes);
			setAINodeNeighbors(node, 28, area958Nodes); break;
		case 78: setAINodeNeighbors(node, 30, area958Nodes);
			setAINodeNeighbors(node, 29, area958Nodes); break;
		}
	}
	std::cout << "-----AREA 957-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area957Nodes) {
		std::cout << "Node Type: " << static_cast<int>(node->nodeType)
			<< " Position: " << node->position
			<< " ID: " << node->id << std::endl;
		//
		switch (node->id) {
		case 4: setAINodeNeighbors(node, 116, area957Nodes); break;
		case 116: setAINodeNeighbors(node, 3, area957Nodes);
			setAINodeNeighbors(node, 3, area957Nodes); break;
		case 115: setAINodeNeighbors(node, 1, area957Nodes);
			setAINodeNeighbors(node, 2, area957Nodes); break;
		}
	}
	/*std::cout << "-----AREA 959-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area959Nodes) {
		std::cout << "Node Type: " << static_cast<int>(node->nodeType)
			<< " Position: " << node->position
			<< " ID: " << node->id << std::endl;
		//31=Right-Ent
		switch (node->id) {
		case 4: setAINodeNeighbors(node, 116, area959Nodes); break;
		case 116: setAINodeNeighbors(node, 3, area959Nodes);
			setAINodeNeighbors(node, 3, area959Nodes); break;
		case 115: setAINodeNeighbors(node, 1, area959Nodes);
			setAINodeNeighbors(node, 2, area959Nodes); break;
		}
	}*/
	/*std::cout << "-----AREA 966-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area966Nodes) {
		std::cout << "Node Type: " << static_cast<int>(node->nodeType)
			<< " Position: " << node->position
			<< " ID: " << node->id << std::endl;
		//43=Middle-Ent,45=RightMiddle-Ent
		switch (node->id) {
		case 1: setAINodeNeighbors(node, 116, area966Nodes); break;
		case 2: setAINodeNeighbors(node, 116, area966Nodes); break;
		case 3: setAINodeNeighbors(node, 116, area966Nodes); break;
		case 116: setAINodeNeighbors(node, 3, area966Nodes);
			setAINodeNeighbors(node, 4, area966Nodes); break;
		case 115: setAINodeNeighbors(node, 1, area966Nodes);
			setAINodeNeighbors(node, 2, area966Nodes); break;
		}
	}*/
	/*std::cout << "-----AREA 968-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area968Nodes) {
		std::cout << "Node Type: " << static_cast<int>(node->nodeType)
			<< " Position: " << node->position
			<< " ID: " << node->id << std::endl;
		//54=Middle-Ent,53=Right-Ent
		switch (node->id) {
		case 1: setAINodeNeighbors(node, 116, area950Nodes); break;
		case 2: setAINodeNeighbors(node, 116, area950Nodes); break;
		case 3: setAINodeNeighbors(node, 116, area950Nodes); break;
		case 116: setAINodeNeighbors(node, 3, area950Nodes);
			setAINodeNeighbors(node, 4, area950Nodes); break;
		case 115: setAINodeNeighbors(node, 1, area950Nodes);
			setAINodeNeighbors(node, 2, area950Nodes); break;
		}
	}
	*/
}

void GameplaySystem::setAINodeNeighbors(std::shared_ptr<AiGraphNode> aiNode1,
										int id,
										std::vector<std::shared_ptr<AiGraphNode>> list) {
	for (std::shared_ptr<AiGraphNode> aiNode2 : list) {
		if (aiNode2->id == id) {
			aiNode1->neighbours.push_back(aiNode2);
			aiNode2->neighbours.push_back(aiNode1);
		}
	}
	
	
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

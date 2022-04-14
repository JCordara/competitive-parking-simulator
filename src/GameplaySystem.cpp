#include "GameplaySystem.h"

#define MAX_SPEED_DRIVE				0.15f
#define MAX_SPEED_ENGINE			0.05f
#define MAX_SPEED_SKID				0.1f
#define MAX_DISTANCE				1.2f
#define MAX_COS_ANGLE_FORWARD		0.94f
#define MAX_COS_ANGLE_UP			0.96f
#define PARKING_TIME				0.4f

#define MAX_SPEED_DRIVE_AI			MAX_SPEED_DRIVE
#define MAX_SPEED_ENGINE_AI			MAX_SPEED_ENGINE
#define MAX_SPEED_SKID_AI			MAX_SPEED_SKID
#define MAX_DISTANCE_AI				3
#define MAX_COS_ANGLE_FORWARD_AI	0.85f
#define MAX_COS_ANGLE_UP_AI			MAX_COS_ANGLE_UP
#define PARKING_TIME_AI				PARKING_TIME


GameplaySystem::GameplaySystem(std::shared_ptr<Scene> scene): 
	scene(scene),
	lastUpdateTime(Time::now())
{
    Events::AiComponentInit.registerHandler<GameplaySystem,
        &GameplaySystem::registerAiComponent>(this);
	Events::CarParked.registerHandler<GameplaySystem,
		&GameplaySystem::registerCarParked>(this);
	Events::NextRound.registerHandler<GameplaySystem,
		&GameplaySystem::setupNewRound>(this);
	Events::NewGame.registerHandler<GameplaySystem,
		&GameplaySystem::setupNewGame>(this);
	Events::EndGame.registerHandler<GameplaySystem,
		&GameplaySystem::cleanUpGame>(this);
	Events::ChangeNumberOfAI.registerHandler<GameplaySystem,
		&GameplaySystem::setNumberOfAI>(this);
	//Events::MainMenu.broadcast();
	gamestate = GameState::MainMenu;
	nextAI_ID = 0;
	startingAi_number = 4;
	currentAi_number = 0;
}

void GameplaySystem::update() {
	vector<weak_ptr<Entity>> toRemove;

	switch (gamestate) {
	case GameState::MainMenu:
		//Do nothing
		break;
	case GameState::Options:
		//Do nothing
		break;
	case GameState::Playing:
		// Update parking AI
		for (auto it = states.begin(); it != states.end(); it++) {
			auto trigger = it->second.trigger.lock();
			if (trigger && (it->second.score < 1)) {
				auto car = it->second.car.lock();
				if ((it->first < 0) ? playerCarCheck(car, trigger) : AICarCheck(car, trigger)) {
					if (it->second.parkedTime.has_value()) {
						if (abs(it->second.parkedTime.value() - Time::now()) > PARKING_TIME) {
							it->second.score = 1;
							toRemove.push_back(it->second.trigger);
							number_of_parking_spots--;
							updateDisplayString();
							car->getComponent<VehicleComponent>()->setDisabled(true);
							Events::CarParked_Official.broadcast(it->second.car);
						}
					}
					else it->second.parkedTime = Time::now();
				}
				else it->second.parkedTime = Time::now();
			}
		}
		//Check player
		if (states.find(-1)->second.score) { // Player has scored
			if (currentAi_number > 1) {//Round 
				setGameState(GameState::RoundEnd);
				Events::RoundEndGUI.broadcast();
			}
			else {//Win
				setGameState(GameState::GameEnd);
				Events::GameEndGUI.broadcast("YOU WIN");
			}
		}
		else {
			bool end = true;
			//If any ai hasnt made it, then game is not over
			for (auto& it : states) if (it.first != -1 && it.second.score < 1) end = false;
			if (end) {//Every single one has parked
				setGameState(GameState::GameEnd);
				Events::GameEndGUI.broadcast("YOU LOSE");
			}
		}
		// Update AI pathing
		if (Time::now() - lastUpdateTime >= 0.25) {
			for (auto& ai : scene->iterate<AiComponent>()) {
				auto p = ai.lock(); if (!p) continue;
				p->update();
			}
			lastUpdateTime = Time::now();
		}
		break;
	case GameState::RoundEnd:
		// Do nothing
		break;
	case GameState::GameEnd:
		//Do nothing
		break;
	}

	// Defer entity deletion so it doesn't occur during PhysX callback
	deleteQueue(toRemove);

}

bool GameplaySystem::playerCarCheck(std::shared_ptr<Entity> car, std::shared_ptr<Entity> trigger) {
	return 		car->getComponent<VehicleComponent>()->getSpeed() < MAX_SPEED_DRIVE &&
				car->getComponent<VehicleComponent>()->getEngineSpeedNormalized() < MAX_SPEED_ENGINE &&
				car->getComponent<VehicleComponent>()->getSkidSpeed() < MAX_SPEED_SKID &&
				//HACK!!!!!!!!
				glm::length(car->getComponent<TransformComponent>()->getGlobalPosition() - trigger->getComponent<TransformComponent>()->getGlobalPosition()) < MAX_DISTANCE &&
				glm::abs(glm::dot(
					glm::normalize(trigger->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(trigger->getComponent<DescriptionComponent>()->getVec3("Forward").value(), 0.f)),
					glm::normalize(car->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(car->getComponent<DescriptionComponent>()->getVec3("Forward").value(), 0.f))
				)) > MAX_COS_ANGLE_FORWARD &&
				glm::abs(glm::dot(
					glm::normalize(trigger->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(trigger->getComponent<DescriptionComponent>()->getVec3("Up").value(), 0.f)),
					glm::normalize(car->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(car->getComponent<DescriptionComponent>()->getVec3("Up").value(), 0.f))
				)) > MAX_COS_ANGLE_UP;
}
bool GameplaySystem::AICarCheck(std::shared_ptr<Entity> car, std::shared_ptr<Entity> trigger) {
	return 		car->getComponent<VehicleComponent>()->getSpeed() < MAX_SPEED_DRIVE_AI &&
				car->getComponent<VehicleComponent>()->getEngineSpeedNormalized() < MAX_SPEED_ENGINE_AI &&
				car->getComponent<VehicleComponent>()->getSkidSpeed() < MAX_SPEED_SKID_AI &&
				//HACK!!!!!!!!
				glm::length(car->getComponent<TransformComponent>()->getGlobalPosition() - trigger->getComponent<TransformComponent>()->getGlobalPosition()) < MAX_DISTANCE_AI &&
				glm::abs(glm::dot(
					glm::normalize(trigger->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(trigger->getComponent<DescriptionComponent>()->getVec3("Forward").value(), 0.f)),
					glm::normalize(car->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(car->getComponent<DescriptionComponent>()->getVec3("Forward").value(), 0.f))
				)) > MAX_COS_ANGLE_FORWARD_AI &&
				glm::abs(glm::dot(
					glm::normalize(trigger->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(trigger->getComponent<DescriptionComponent>()->getVec3("Up").value(), 0.f)),
					glm::normalize(car->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(car->getComponent<DescriptionComponent>()->getVec3("Up").value(), 0.f))
				)) > MAX_COS_ANGLE_UP_AI;
}

void GameplaySystem::defineMap(
	std::string graphFile,
	std::vector<instancedTransformation> parkingSpots,
	std::vector<instancedTransformation> emptyParkingSpots
) {
	//Saving data for later:
	// AI graph
	readAiGraph(graphFile);
	// Parking Spots
	possibleParkingSpots = emptyParkingSpots;
	// Adding in the propcars to parking spaces (Only ones that cant be a destonation space)
	for (auto parkingSpot = parkingSpots.begin(); parkingSpot != parkingSpots.end(); parkingSpot++)
		Events::AddPropCar.broadcast("Constaint propcar", *parkingSpot);
	cleanMap();
}

void GameplaySystem::resetMapWithNumberOfEmptyParkingSpaces(unsigned int numberOfParkingSpots) {
	//Generate random nodes
	vector<weak_ptr<Entity>> toRemove;
	std::vector<int> spotChoice = Random::permutationInt(0, possibleParkingSpots.size(), numberOfParkingSpots);
	parking = std::vector<bool>(possibleParkingSpots.size());
	for (int i = 0; i < spotChoice.size(); i++) parking[spotChoice[i]] = true;
	std::vector<bool> parkingUpdated = std::vector<bool>(possibleParkingSpots.size());
	// Reset or delete the cars
	for (auto wp_rb : scene->iterate<RigidbodyComponent>()) {
		auto rb = wp_rb.lock(); if (!rb) continue;
		auto ent = rb->getEntity();
		if (auto des = ent->getComponent<DescriptionComponent>()) {
			if (auto name = des->getString("Name")) {
				if (name.value() == "Constaint propcar") {// Needs to be placed back to its og transform
					ent->getComponent<TransformComponent>()->setLocalPosition(des->getVec3("Spawn Position").value());
					ent->getComponent<TransformComponent>()->setLocalRotation(glm::radians(des->getRealNumber("Spawn Y-Rotation").value()), glm::vec3(0.f,1.f,0.f));
				}
				else if (prefix("Temporary propcar : ", name.value())) {
					int number = std::stoi(name.value().substr(string("Temporary propcar : ").length()));
					if (parking[number]) // Needs to be removed
						toRemove.push_back(ent);//scene->removeEntity(ent);
					else { //Just needs to be placed back to its og transform
						ent->getComponent<TransformComponent>()->setLocalPosition(des->getVec3("Spawn Position").value());
						ent->getComponent<TransformComponent>()->setLocalRotation(glm::radians(des->getRealNumber("Spawn Y-Rotation").value()), glm::vec3(0.f, 1.f, 0.f));
						parkingUpdated[number] = true;
					}
				}
			}
		}
	}
	//Delete Parking Spots
	for (auto wp_rb : scene->iterate<VolumeTriggerComponent>()) {
		auto rb = wp_rb.lock(); if (!rb) continue;
		auto ent = rb->getEntity();
		if (auto des = ent->getComponent<DescriptionComponent>()) {
			if (auto name = des->getString("Name")) {
				if (prefix("Temporary parkingspot : ", name.value())) {
					toRemove.push_back(ent);//scene->removeEntity(ent);
				}
			}
		}
	}

	deleteQueue(toRemove);

	//Fill the spots will either a triggerbox or a car
	for (int i = 0, index = 0; i < parkingUpdated.size(); i++) {
		if ((!parkingUpdated[i])) {
			if (parking[i]) // Add a trigger box
				Events::AddParkingSpace.broadcast("Temporary parkingspot : " + std::to_string(i), possibleParkingSpots[i]);
			else //Add a car
				Events::AddPropCar.broadcast("Temporary propcar : " + std::to_string(i), possibleParkingSpots[i]);
		}	
	}
//---------------------------------------------------------------------------------------------------------------------
	//reset Vechicles
	int numberOfAI = 0;
	for (auto wp_vc : scene->iterate<VehicleComponent>()) {
		auto vc = wp_vc.lock(); if (!vc) continue;
		auto ent = vc->getEntity();
		if (auto des = ent->getComponent<DescriptionComponent>()) {
			if (auto name = des->getString("Name")) {
				if ("Player Car" == name.value()) {
					ent->getComponent<TransformComponent>()->setLocalPosition(des->getVec3("Spawn Position").value());
					ent->getComponent<TransformComponent>()->setLocalRotation(glm::radians(des->getRealNumber("Spawn Y-Rotation").value()), glm::vec3(0.f, 1.f, 0.f));
					auto st = states.find(-1);
					st->second.score = 0;
					st->second.trigger = std::weak_ptr<Entity>();
					ent->getComponent<VehicleComponent>()->setDisabled(false);
				}
				else if(prefix("AI Car : ", name.value())) {
					int number = std::stoi(name.value().substr(string("AI Car : ").length()));
					ent->getComponent<AiComponent>()->resetAi();
					auto st = states.find(number);
					st->second.score = 0;
					st->second.trigger = std::weak_ptr<Entity>();
					numberOfAI++;
					ent->getComponent<VehicleComponent>()->setDisabled(false);
				}
			}
		}
	}
	CarState temp;
	for (; numberOfAI < numberOfParkingSpots; numberOfAI++) {
		Events::AddAICar.broadcast("AI Car : " + std::to_string(nextAI_ID));
		states.insert(std::pair<int, CarState>(nextAI_ID, temp));
		nextAI_ID++;
	}

//---------------------------------------------------------------------------------------------------------------------
	// Reset Nodes
	for (auto node : aiGlobalNodes) {
		node->nodeTaken = false;
	}
}

void GameplaySystem::cleanMap() {
	vector<weak_ptr<Entity>> toRemove;

	//Delete the cars
	for (auto wp_rb : scene->iterate<RigidbodyComponent>()) {
		auto rb = wp_rb.lock(); if (!rb) continue;
		auto ent = rb->getEntity();
		if (auto des = ent->getComponent<DescriptionComponent>()) {
			if (auto name = des->getString("Name")) {
				if (prefix("Temporary propcar : ", name.value())) {
					toRemove.push_back(ent);//scene->removeEntity(ent);
				}
			}
		}
	}
	//Delete Parking Spots
	for (auto wp_vt : scene->iterate<VolumeTriggerComponent>()) {
		auto vt = wp_vt.lock(); if (!vt) continue;
		auto ent = vt->getEntity();
		if (auto des = ent->getComponent<DescriptionComponent>()) {
			if (auto name = des->getString("Name")) {
				if (prefix("Temporary parkingspot : ", name.value())) {
					toRemove.push_back(ent);//scene->removeEntity(ent);
				}
			}
		}
	}
	//Delete the cars
	for (auto wp_vc : scene->iterate<VehicleComponent>()) {
		auto vc = wp_vc.lock(); if (!vc) continue;
		auto ent = vc->getEntity();
		if (auto des = ent->getComponent<DescriptionComponent>()) {
			if (auto name = des->getString("Name")) {
				if ("Player Car" == name.value()) {
					vc->setDisabled(true);
					ent->getComponent<TransformComponent>()->setLocalPosition(des->getVec3("Spawn Position").value());
					ent->getComponent<TransformComponent>()->setLocalRotation(glm::radians(des->getRealNumber("Spawn Y-Rotation").value()), glm::vec3(0.f, 1.f, 0.f));
				}
				else if (prefix("AI Car : ", name.value())) {
					toRemove.push_back(ent); //scene->removeEntity(ent);
				}
			}
		}
	}
	states.clear();
	CarState temp;
	states.insert(std::pair<int, CarState>(-1, temp));
	deleteQueue(toRemove);
}

void GameplaySystem::removeBottomAI(unsigned int num) {
	using type = std::pair<
		std::shared_ptr<Entity>,
		std::pair<int, int>
	>;
	vector<weak_ptr<Entity>> toRemove;
	std::vector<type> listOfAIs;
	for (auto wp_vc : scene->iterate<VehicleComponent>()) {
		auto vc = wp_vc.lock(); if (!vc) continue;
		auto ent = vc->getEntity();
		if (auto des = ent->getComponent<DescriptionComponent>()) {
			if (auto name = des->getString("Name")) {
				if (prefix("AI Car : ", name.value())) {
					int number = std::stoi(name.value().substr(string("AI Car : ").length()));
					auto score = states.find(number);
					if (score == states.end())
						throw std::exception("Player Score Not Found");
					type element = type(ent, std::pair<int, int>(number,score->second.score));
					// Insertion Sort
					bool inserted = false;
					for (std::vector<type>::iterator it = listOfAIs.begin(); it != listOfAIs.end(); it++)
						if (it->second.second > element.second.second) {
							listOfAIs.insert(it, element);
							inserted = true;
							break;
						}
					if (!inserted) listOfAIs.push_back(element);
				}
			}
		}
	}
	int i = 0;
	for (std::vector<type>::iterator it = listOfAIs.begin(); i < num && it != listOfAIs.end(); it++, i++) {
		toRemove.push_back(it->first);//scene->removeEntity(it->first);
		states.erase(it->second.first);
	}
	deleteQueue(toRemove);

}

void GameplaySystem::deleteQueue(vector<weak_ptr<Entity>> e) {
	if (e.size() > 0) {
		for (auto wp : e) {
			auto e = wp.lock(); if (!e) continue;
			auto p = e->parent().lock(); if (!p) return;
			p->removeChild(e);
		}
		e.clear();
		scene->untrackDeletedComponents();
	}
}

void GameplaySystem::setupNewGame() {
	currentAi_number = startingAi_number;
	number_of_parking_spots = currentAi_number;
	updateDisplayString();
	resetMapWithNumberOfEmptyParkingSpaces(currentAi_number);
	Events::GameGUI.broadcast();
	setGameState(GameState::Playing);
}

void GameplaySystem::setupNewRound() {
	removeBottomAI(1);
	currentAi_number = currentAi_number - 1;
	number_of_parking_spots = currentAi_number;
	updateDisplayString();
	resetMapWithNumberOfEmptyParkingSpaces(currentAi_number);
	Events::GameGUI.broadcast();
	setGameState(GameState::Playing);
}
void GameplaySystem::cleanUpGame() {
	cleanMap();
	Events::MainMenu.broadcast();
	if(gamestate != GameState::MainMenu)
		Events::SetMenuMusic.broadcast();
	setGameState(GameState::MainMenu);
}

GameplaySystem::~GameplaySystem() {}

void GameplaySystem::setNumberOfAI(int num) { startingAi_number = num; }

void GameplaySystem::registerAiComponent(AiComponent& component) {
	component.setGameplaySystem(
		dynamic_pointer_cast<GameplaySystem>(shared_from_this()));
}


void GameplaySystem::registerCarParked(weak_ptr<Entity> VehcleEntity, weak_ptr<Entity> TriggerEntity) {
	
	// Get shared pointers
	auto vehicle = VehcleEntity.lock();
	auto trigger = TriggerEntity.lock();
	if (!vehicle || !trigger) return;

	if (gamestate == GameState::Playing) {
		if (auto des = vehicle->getComponent<DescriptionComponent>()) {
			if (auto name = des->getString("Name")) {
				if (name.value() == "Player Car") {
					auto p = states.find(-1);
					p->second.car = VehcleEntity;
					p->second.trigger = TriggerEntity;
				}
				else if (prefix("AI Car : ", name.value())) {
					int number = std::stoi(name.value().substr(string("AI Car : ").length()));
					auto p = states.find(number);
					p->second.car = VehcleEntity;
					p->second.trigger = TriggerEntity;
				}
			}
		}
	}

}

void GameplaySystem::registerCarUnParked(weak_ptr<Entity> VehcleEntity) {

	// Get shared pointers
	auto vehicle = VehcleEntity.lock();
	if (!vehicle) return;

	if (gamestate == GameState::Playing) {
		if (auto des = vehicle->getComponent<DescriptionComponent>()) {
			if (auto name = des->getString("Name")) {
				if (name.value() == "Player Car") {
					//states.find(-1)->second.car = 
				}
				else if (prefix("AI Car : ", name.value())) {
					int number = std::stoi(name.value().substr(string("AI Car : ").length()));
					//states.find(number)->second.second = true;
				}
			}
		}
	}

}

void GameplaySystem::setNodeType(std::string nodeType, std::shared_ptr<AiGraphNode> aiNode) {
	// spawn entrance traversal parking spot
	if (!nodeType.compare("spawn")) {
		aiNode->nodeType = AiGraphNode::NodeType::SPAWN;
		aiNode->nodeSpeed = 0.25;
		aiNode->nodeThreshold = 3.5;
	}
	if (!nodeType.compare("entrance")) {
		aiNode->nodeType = AiGraphNode::NodeType::LOTENTRANCE;
		aiNode->nodeSpeed = 0.35;
		aiNode->nodeThreshold = 3.5;
	}
	if (!nodeType.compare("traversal")) {
		aiNode->nodeType = AiGraphNode::NodeType::TRAVERSAL;
		aiNode->nodeSpeed = 0.55;
		aiNode->nodeThreshold = 5;
	}
	if (!nodeType.compare("parking")) {
		aiNode->nodeType = AiGraphNode::NodeType::PARKINGSTALL;
		aiNode->nodeSpeed = 0.15;
		aiNode->nodeThreshold = 1.5;
	}
}

std::shared_ptr<AiGraphNode> GameplaySystem::addAINode(const std::string nodeType, int inId, glm::vec3 nodePos) {
	std::shared_ptr<AiGraphNode> aiNode = std::make_shared<AiGraphNode>();
	setNodeType(nodeType, aiNode);
	aiNode->id = inId;
	aiNode->position = nodePos;
	aiGlobalNodes.push_back(aiNode);
	return aiNode;
}

void GameplaySystem::readAiGraph(string filepath) {

	std::ifstream file(filepath);
	string str;
	vector<string> tokenizedLine;
	unordered_map<shared_ptr<AiGraphNode>, vector<int>> nodeNeighboursMap;
	size_t pos = 0;
	string delimiter = " ";

	while (std::getline(file, str)) {
		tokenizedLine.clear();
		std::vector<int> lineNodeNeighbours;

		// Tokenize input string
		while ((pos = str.find(delimiter)) != string::npos) {
			tokenizedLine.push_back(str.substr(0, pos));
			str.erase(0, pos + delimiter.length());
		}

		// Node neighbors are remaining in input string
		std::string delimiter = ",";
		while ((pos = str.find(delimiter)) != string::npos) {
			lineNodeNeighbours.push_back(std::stoi(str.substr(0, pos)));
			str.erase(0, pos + delimiter.length());
		}
		lineNodeNeighbours.push_back(std::stoi(str));

		std::shared_ptr<AiGraphNode> nde = addAINode(
			tokenizedLine[0],
			std::stoi(tokenizedLine[1]),
			glm::vec3(
				std::stof(tokenizedLine[2]),
				std::stof(tokenizedLine[3]),
				std::stof(tokenizedLine[4])
			)
		);
		nodeNeighboursMap[nde] = lineNodeNeighbours;
	}

	// Second pass to set neighbors
	for (auto it : nodeNeighboursMap) {
		setNeigbours(it.first, it.second);
	}

}

void GameplaySystem::setNeigbours(std::shared_ptr<AiGraphNode> nodePrime, std::vector<int> nodeNeighbours) {
	for (int nodeId : nodeNeighbours) {
		nodePrime->neighbours.push_back(retrieveNode(nodeId));
	}
}

std::shared_ptr<AiGraphNode> GameplaySystem::retrieveNode(int inId) {
	for (std::shared_ptr<AiGraphNode> node : aiGlobalNodes) {
		if (node->id == inId) {
			return node;
		}
	}
	return nullptr;
}


std::vector<std::shared_ptr<AiGraphNode>> GameplaySystem::getAreaNodes(int nodeAreaCode) {
	switch (nodeAreaCode) {
	case 950: return area950Nodes; break;
	case 951: return area951Nodes; break;
	case 952: return area952Nodes; break;
	case 953: return area953Nodes; break;
	case 954: return area954Nodes; break;
	case 955: return area955Nodes; break;
	case 956: return area956Nodes; break;
	case 957: return area957Nodes; break;
	case 958: return area958Nodes; break;
	case 959: return area959Nodes; break;
	case 960: return area960Nodes; break;
	case 961: return area961Nodes; break;
	case 962: return area962Nodes; break;
	case 963: return area963Nodes; break;
	case 964: return area964Nodes; break;
	case 965: return area965Nodes; break;
	case 966: return area966Nodes; break;
	case 967: return area967Nodes; break;
	case 968: return area968Nodes; break;
	case 969: return area969Nodes; break;
	case 970: return area970Nodes; break;
	default:
		break;
	}
}

void GameplaySystem::testPrintAINodes() {
	//std::cout << "-----AREA 970-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area970Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//59=Right-Ent,60=Bottom-Ent,61=Top-Ent,62=Left-Ent
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
			//All spawn nodes are the same
			//setAINodeNeighbors(node, 59, area970Nodes);
			//setAINodeNeighbors(node, 60, area970Nodes);
			//setAINodeNeighbors(node, 61, area970Nodes);
			//setAINodeNeighbors(node, 62, area970Nodes);
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
			setAINodeNeighbors(node, 4, area950Nodes);
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
		case 8: setAINodeNeighbors(node, 111, area952Nodes);
			setAINodeNeighbors(node, 9, area952Nodes); break;
		case 9: setAINodeNeighbors(node, 15, area953Nodes); break;
		case 10: setAINodeNeighbors(node, 5, area951Nodes); break;
		case 11: setAINodeNeighbors(node, 13, area953Nodes); break;
		case 12: setAINodeNeighbors(node, 49, area967Nodes); break;
		case 110: setAINodeNeighbors(node, 114, area952Nodes); break;
		case 111: setAINodeNeighbors(node, 8, area952Nodes);
			setAINodeNeighbors(node, 9, area952Nodes);
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
		case 23: setAINodeNeighbors(node, 22, area955Nodes); break;
		case 25: setAINodeNeighbors(node, 28, area958Nodes); break;
		case 26: setAINodeNeighbors(node, 31, area959Nodes); break;
		case 74: setAINodeNeighbors(node, 75, area956Nodes);
			setAINodeNeighbors(node, 24, area956Nodes);
			setAINodeNeighbors(node, 23, area956Nodes);
			setAINodeNeighbors(node, 25, area956Nodes); break;
		case 75:setAINodeNeighbors(node, 26, area956Nodes);
			setAINodeNeighbors(node, 25, area956Nodes); break;
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
		//21=Right-Ent,22=Top-Ent,73=Tra,125=Park
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
	//std::cout << "-----AREA 957-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area957Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//27-Ent,76-Tra,126-Park
		switch (node->id) {
		case 76: setAINodeNeighbors(node, 27, area957Nodes);
			setAINodeNeighbors(node, 126, area957Nodes); break;
		}
	}
	//std::cout << "-----AREA 959-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area959Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//31=Right-Ent,32=TopRight-Ent,33=Topleft-Ent,79=Right-Tra,80=Middle-Tra,81=Left-Tra
		//127=Right-Park,128=Left-Park
		switch (node->id) {
		case 32: setAINodeNeighbors(node, 34, area960Nodes); break;
		case 33: setAINodeNeighbors(node, 35, area961Nodes); break;
		case 79: setAINodeNeighbors(node, 80, area959Nodes);
			setAINodeNeighbors(node, 31, area959Nodes);
			setAINodeNeighbors(node, 32, area959Nodes); break;
		case 80: setAINodeNeighbors(node, 81, area959Nodes);
			setAINodeNeighbors(node, 32, area959Nodes);
			setAINodeNeighbors(node, 127, area959Nodes); break;
		case 81: setAINodeNeighbors(node, 33, area959Nodes);
			setAINodeNeighbors(node, 128, area959Nodes); break;
		}
	}
	//std::cout << "-----AREA 960-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area960Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//34=Bottom-Ent
		switch (node->id) {
		case 82: setAINodeNeighbors(node, 34, area960Nodes);
			setAINodeNeighbors(node, 129, area960Nodes); break;
		}
	}
	//std::cout << "-----AREA 961-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area961Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//35=Bottom-Ent,36=Top-Ent,37=Left-Ent,83-Tra
		switch (node->id) {
		case 35: setAINodeNeighbors(node, 37, area961Nodes); break;
		case 36: setAINodeNeighbors(node, 37, area961Nodes);
			setAINodeNeighbors(node, 38, area962Nodes); break;
		case 37: setAINodeNeighbors(node, 42, area964Nodes); break;
		case 83: setAINodeNeighbors(node, 35, area961Nodes);
			setAINodeNeighbors(node, 36, area961Nodes);
			setAINodeNeighbors(node, 37, area961Nodes); break;
		}
	}
	//std::cout << "-----AREA 962-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area962Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//38=Bottom-Ent,39=Top-Ent,40=Right-Ent,84=Right-Tra,85=Left-Tra
		//130=Left-Park,131=Right-Park
		switch (node->id) {
		case 38: setAINodeNeighbors(node, 130, area962Nodes); break;
		case 39: setAINodeNeighbors(node, 55, area968Nodes); break;
		case 40: setAINodeNeighbors(node, 41, area963Nodes); break;
		case 84: setAINodeNeighbors(node, 39, area962Nodes);
			setAINodeNeighbors(node, 40, area962Nodes);
			setAINodeNeighbors(node, 131, area962Nodes); break;
		case 85: setAINodeNeighbors(node, 84, area962Nodes);
			setAINodeNeighbors(node, 38, area962Nodes);
			setAINodeNeighbors(node, 39, area962Nodes);
			setAINodeNeighbors(node, 130, area962Nodes); break;
		}
	}
	//std::cout << "-----AREA 963-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area963Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//41=Left-Ent,86=Left-Tra,87=Right-Tra,132=Right-Park,133=Left-Park
		switch (node->id) {
		case 86: setAINodeNeighbors(node, 87, area963Nodes);
			setAINodeNeighbors(node, 41, area963Nodes);
			setAINodeNeighbors(node, 133, area963Nodes); break;
		case 87: setAINodeNeighbors(node, 132, area963Nodes); break;
		}
	}
	//std::cout << "-----AREA 964-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area964Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//42=Right-Ent,88=Bottom-Tra,89=Top-Tra,134=Top-Park,135=Bottom-Park
		switch (node->id) {
		case 35: setAINodeNeighbors(node, 37, area964Nodes); break;
		case 88: setAINodeNeighbors(node, 89, area964Nodes);
			setAINodeNeighbors(node, 42, area964Nodes);
			setAINodeNeighbors(node, 134, area964Nodes); break;
		case 89: setAINodeNeighbors(node, 42, area964Nodes);
			setAINodeNeighbors(node, 135, area964Nodes); break;
		}
	}
	//std::cout << "-----AREA 966-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area966Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//43=Middle-Ent,45=RightMiddle-Ent,46=Right-Ent,44=Left-Ent
		//98=Left-Tra,94=MiddleLeft-Tra,95=Middle-Tra,96=MiddleRight-Tra,97=Right-Tra
		switch (node->id) {
		case 44: setAINodeNeighbors(node, 47, area965Nodes); break;
		case 98: setAINodeNeighbors(node, 44, area966Nodes); break;
		case 94: setAINodeNeighbors(node, 98, area966Nodes);
			setAINodeNeighbors(node, 43, area966Nodes); break;
		case 95: setAINodeNeighbors(node, 94, area966Nodes);
			setAINodeNeighbors(node, 96, area966Nodes);
			setAINodeNeighbors(node, 43, area966Nodes); break;
		case 96: setAINodeNeighbors(node, 97, area966Nodes);
			setAINodeNeighbors(node, 45, area966Nodes); break;
		case 97: setAINodeNeighbors(node, 45, area966Nodes);
			setAINodeNeighbors(node, 46, area966Nodes); break;
		}
	}
	//std::cout << "-----AREA 968-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area968Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//54=Middle-Ent,53=Right-Ent,55=MiddleLeft-Ent,56=Left-Ent,52=Right-Ent
		//107=Left-Tra,106=SecondLeft-Tra,105=MiddleLeft-Tra,104=MiddleRight-Tra,103=SecondRight-Tra,102=Right-Tra
		switch (node->id) {
		case 56: setAINodeNeighbors(node, 57, area969Nodes); break;
		case 52: setAINodeNeighbors(node, 50, area967Nodes); break;
		case 105: setAINodeNeighbors(node, 106, area968Nodes);
			setAINodeNeighbors(node, 104, area968Nodes);
			setAINodeNeighbors(node, 54, area968Nodes); break;
		case 104: setAINodeNeighbors(node, 103, area968Nodes);
			setAINodeNeighbors(node, 54, area968Nodes); break;
		case 106: setAINodeNeighbors(node, 107, area968Nodes); break;
		case 107: setAINodeNeighbors(node, 56, area968Nodes); break;
		case 103: setAINodeNeighbors(node, 102, area968Nodes); break;
		case 102: setAINodeNeighbors(node, 52, area968Nodes); break;
		}
	}
	//std::cout << "-----AREA 969-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area969Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//57=Top-Ent,58=Bottom-Ent,108=Tra
		switch (node->id) {
		case 58: setAINodeNeighbors(node, 48, area965Nodes); break;
		case 108: setAINodeNeighbors(node, 57, area969Nodes);
			setAINodeNeighbors(node, 58, area969Nodes); break;
		}
	}
	//std::cout << "-----AREA 967-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area967Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//49=Middle-Ent,50=top-Ent,51=Bottom-Ent,99=Middle-Tra,100=Top-Tra,101=Bottom-Tra
		switch (node->id) {
		case 51: setAINodeNeighbors(node, 46, area966Nodes); break; //adofiugh
		case 99: setAINodeNeighbors(node, 100, area967Nodes);
			setAINodeNeighbors(node, 101, area967Nodes);
			setAINodeNeighbors(node, 49, area967Nodes); break;
		case 100: setAINodeNeighbors(node, 50, area967Nodes); break;
		case 101:setAINodeNeighbors(node, 49, area967Nodes);
			setAINodeNeighbors(node, 51, area967Nodes); break;
		}
	}
	//std::cout << "-----AREA 965-----" << std::endl;
	for (std::shared_ptr<AiGraphNode> node : area965Nodes) {
		//std::cout << "Node Type: " << static_cast<int>(node->nodeType)
		//	<< " Position: " << node->position
		//	<< " ID: " << node->id << std::endl;
		//47=Right-Ent,48=Left-Ent
		//90=Right-Tra,91=MiddleRight-Tra,93=MiddleLeft-Tra,92=Left-Tra
		//136=RightPark,137=Left-Park
		switch (node->id) {
		case 90: setAINodeNeighbors(node, 47, area965Nodes); break;
		case 91: setAINodeNeighbors(node, 90, area965Nodes);
			setAINodeNeighbors(node, 93, area965Nodes);
			setAINodeNeighbors(node, 47, area965Nodes); break;
		case 92: setAINodeNeighbors(node, 48, area965Nodes);
			setAINodeNeighbors(node, 137, area965Nodes); break;
		case 93: setAINodeNeighbors(node, 92, area965Nodes);
			setAINodeNeighbors(node, 136, area965Nodes); break;
		}
	}
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
/*
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
}*/

// Wasn't working, so now it's just a "contains" function as a quick fix
bool prefix(const string& prefix, const string& base) {
	return (base.find(prefix) != string::npos);
	// auto res = std::mismatch(prefix.begin(), prefix.end(), base.begin());
	// return (res.first == prefix.end());
}

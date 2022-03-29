#include "AiComponent.h"
#include "GameplaySystem.h"

// Default constructor
AiGraphNode::AiGraphNode() {

}

// Normal constructor for AI Component
// Calls the base component constructor then broadcasts its creation
// Sets the AI's spawn node, picks a parking lot entrance to go to
// then starts accelerating
// The pickRandGoalNode() call, calls A* on its own
AiComponent::AiComponent(shared_ptr<Entity> parent) 
    : BaseComponent(parent)
{
	//for (std::shared_ptr<AiGraphNode> node : gameplaySystem->area970Nodes) {
	//	std::cout << "NODE ID" << node->id << std::endl;
	//	std::cout << "AREA CODE" << node->areaCode << std::endl;
	//}
	Events::AiComponentInit.broadcast(*this);
	setupAreaMap();
	setSpawnNode();
	pickRandEntranceNode();
	accelForwards();
}

// Needed method for Entity typing
ComponentEnum AiComponent::getType() {
    return ComponentEnum::ai;
}

// Destructor
AiComponent::~AiComponent() {
    // Nothing to do here yet
}

// The update method that is called continously for the AI
// Switches between function calls depending on current state
void AiComponent::update() {
	if (state == States::SEARCH) {
		searchState();
	}
	else if (state == States::RECOVERY) {
		recoveryState();
	}
}

// A* algorithm based on custom node structure, works by traversing neighbors
// Should be called after reaching a goal node, takes in chosen node
// Traverses the graph via neighbours from the currentNode to the goalNode
// AI Car belongs to whatever node it is currently going to, i.e. currentNode
void AiComponent::aStar(std::shared_ptr<AiGraphNode> goalNode) {

	std::vector<std::shared_ptr<AiGraphNode>> openList;
	std::vector<std::shared_ptr<AiGraphNode>> closedList;

	// Initialize open list with starting node
	currentNode->parentNode = nullptr;
	currentNode->g = 0;
	openList.push_back(currentNode);

	std::shared_ptr<AiGraphNode> Q = currentNode; // just to start
	while (true) {
		if (openList.size() == 0) break;
		int index = 0;
		if (openList.size() > 0) {
			Q = openList[0]; // Just to start
		}
		// Find next node closest to goal
		for (int i = 0; i < openList.size(); i++) {
			if (getFValue(openList[i]) < getFValue(Q)) {
				Q = openList[i];
				index = i;
			}
		}
		openList.erase(openList.begin()+index);
		if (Q->id == goalNode->id) break; // return Q
		closedList.push_back(Q);
		for each (std::shared_ptr<AiGraphNode> neighbor in Q->neighbours){
			neighbor->g = Q->g + getHValue(Q, neighbor);
			neighbor->h = getHValue(neighbor, goalNode); //hueristic

			bool skip = false;
			// If this node has already been visited and has the same
			// or a better path, ignore this node
			for each (std::shared_ptr<AiGraphNode> openNode in openList) {
				if (openNode->position == neighbor->position) {
					if (getFValue(openNode) <= getFValue(neighbor)) {
						skip = true;
					}
				}
			}
			// If this node has already been visited and has the same
			// or a better path, ignore this node
			for each (std::shared_ptr<AiGraphNode> closedNode in closedList) {
				if (closedNode->position == neighbor->position) {
					if (getFValue(closedNode) <= getFValue(neighbor)) {
						skip = true;
					}
				}
			}
			if (!skip) {
				neighbor->parentNode = Q;
				openList.push_back(neighbor);
			}
			// Sort so that the best choice is at start of list
			std::sort(openList.begin(), openList.end());
		}
	}
	//exits when Q is the goal
	std::shared_ptr<AiGraphNode> tempNode = Q;
	nodeQueue.push_back(tempNode);//goal node
	while (true) {
		if (tempNode->parentNode != nullptr) {
			tempNode = tempNode->parentNode;
			// Don't add node already reached
			if (tempNode->id != currentNode->id) {
				nodeQueue.insert(nodeQueue.begin(), tempNode);
			}
		}
		else {
			break;
		}
	}
}

// Helper function for A*
// This is just a get distance method really
float AiComponent::getHValue(std::shared_ptr<AiGraphNode> node1, std::shared_ptr<AiGraphNode> node2) {
	float distX = std::abs(node2->position.x - node1->position.x);
	float distY = std::abs(node2->position.y - node1->position.y);
	float distZ = std::abs(node2->position.z - node1->position.z);
	// h is estimated cost of shortest path to goal node
	return distX + distY + distZ;
}
// Helper function for A*
// Returns a nodes g+h values
float AiComponent::getFValue(std::shared_ptr<AiGraphNode> node) {
	// f is distance between start, node and goal node
	return node->g + node->h;
}

// Set the spawn Node for the AI
// Chooses the first available Node in the global node list
void AiComponent::setSpawnNode() {
	for each (std::shared_ptr<AiGraphNode> node in gameplaySystem->aiGlobalNodes) {
		if (node->nodeType == AiGraphNode::NodeType::SPAWN && !(node->nodeTaken)) {
			currentNode = node;
			node->nodeTaken = true;
			//node->spawnAiComponent = entity->shared_from_this();
			entity->getComponent<TransformComponent>()->setLocalPosition(node->position);
			return;
		}
	}
}

// Resets the AI to a chosen spawn position and search state
void AiComponent::resetAi() {
	setSpawnNode();
	visitedAreas.clear();
	switchState(States::SEARCH);
	pickRandEntranceNode();
}

// Simple method for switching states
// In case of recovery, don't set the last state to Recovery
void AiComponent::switchState(States newState) {
	lastState = state;
	state = newState;
}

// Finds the closest parking space node
// Start node is the entrance to the parking lot
// Traverses the lot through neighbors to make a list and find a parking stall
bool AiComponent::pickClosestParkingNode(std::shared_ptr<AiGraphNode> startNode) {
	std::vector<std::shared_ptr<AiGraphNode>> parkingLot;
	std::vector<std::shared_ptr<AiGraphNode>> visited;
	// First adds the first neighbor in the lot to the
	for each (std::shared_ptr<AiGraphNode> node in startNode->neighbours) {
		if (node->nodeType == AiGraphNode::NodeType::TRAVERSAL) {
			parkingLot.push_back(node);
			break; // Only need one node to start traversal
		}
		// If the only neighbour happens to be a parking stall end there
		else if (node->nodeType == AiGraphNode::NodeType::PARKINGSTALL) {
			currentNode = node;
			return true;
		}
	}
	visited.push_back(startNode);
	// Traverse the lot sub-graph from the starting inner lot node
	for (int i = 0; i < parkingLot.size(); i++) {
		for each (std::shared_ptr<AiGraphNode> neighbour in parkingLot[i]->neighbours) {
			bool isInLot = parkingLot[i]->nodeType == AiGraphNode::NodeType::TRAVERSAL;
			bool isPark = parkingLot[i]->nodeType == AiGraphNode::NodeType::PARKINGSTALL;
			bool isNotTaken = !parkingLot[i]->nodeTaken;
			if (isInLot) {
				if (std::count(visited.begin(), visited.end(), neighbour) == 0) {
					parkingLot.push_back(neighbour);
				}
			}
			else if (isPark && isNotTaken) {
				nodeQueue.clear(); // clear path for A*
				std::cout << "FOUND PARKING NODE: " << parkingLot[i]->id << "PARKING NODE AREA: " << parkingLot[i]->areaCode << std::endl;
				aStar(parkingLot[i]); // Sets path
				return true;
			}
		}
		visited.push_back(parkingLot[i]);
	}
	return false;
}

void AiComponent::setupAreaMap() {
	areaMap[950] = std::vector{951,958,968,970};
	areaMap[951] = std::vector{950,952,968};
	areaMap[952] = std::vector{951,953,967,970};
	areaMap[953] = std::vector{952,954,966};
	areaMap[954] = std::vector{953,955,966,970};
	areaMap[955] = std::vector{954,956};
	areaMap[956] = std::vector{955,958,959,970};
	areaMap[957] = std::vector{958};
	areaMap[958] = std::vector{950,956,957};
	areaMap[959] = std::vector{956,960,961};
	areaMap[960] = std::vector{959};
	areaMap[961] = std::vector{959,962,964};
	areaMap[962] = std::vector{961,963,968};
	areaMap[963] = std::vector{962};
	areaMap[964] = std::vector{961};
	areaMap[965] = std::vector{966,969};
	areaMap[966] = std::vector{953,954,965,967};
	areaMap[967] = std::vector{952,966,968};
	areaMap[968] = std::vector{950,951,962,967,969};
	areaMap[969] = std::vector{965,968};
	areaMap[970] = std::vector{950,952,954,956};
}

// Traverses the global node list to find a random entrance to go to
// Calls A* after done with chosen node
void AiComponent::pickRandEntranceNode() {
	// Randomly pick a node from the entrances to parking lots
	std::vector<std::shared_ptr<AiGraphNode>> nodes;
	std::cout << "CURRENT AREA CODE: " << currentNode->areaCode << std::endl;
	for (int area : areaMap[currentNode->areaCode]) {
		std::cout << "SEARCH AREA CODE: " << area << std::endl;
		if (std::count(visitedAreas.begin(), visitedAreas.end(), area) == 0) {
			std::shared_ptr<AiGraphNode> closestNode; float closestDistance;
			//Find neighboring nodes
			for (std::shared_ptr<AiGraphNode> node1 : gameplaySystem->getAreaNodes(area)) {
				for (std::shared_ptr<AiGraphNode> node2 : gameplaySystem->getAreaNodes(currentNode->areaCode)) {
					if (std::count(node1->neighbours.begin(), node1->neighbours.end(), node2) > 0) {
						std::cout << "FOUND NEIGHBOR: " << node1->id << std::endl;
						std::cout << "FOR NODE: " << node2->id << std::endl;
						std::cout << "CURRENT NODE: " << currentNode->id << " CURRENT AREA CODE: " << currentNode->areaCode << std::endl;
						closestNode = node1;
					}
				}
				/*if (node->nodeType == AiGraphNode::NodeType::LOTENTRANCE) {
					if (closestNode == nullptr ||
						glm::distance(currentNode->position, node->position) < closestDistance) {
						closestDistance = glm::distance(currentNode->position, node->position);
						closestNode = node;
					}
				}*/
			}
			nodes.push_back(closestNode);
		}
	}
	if (nodes.size() == 0) return;
	int randIntCeiling = nodes.size();
	std::srand(Time::now() + (double)entity->id()); // Get AI picking differently
	// Should give number between 0 and vector.size()-1
	int pick = rand() % randIntCeiling;
	std::cout << "PICKED ID: " << nodes[pick]->id << " PICKED AREA: " << nodes[pick]->areaCode << std::endl;
	aStar(nodes[pick]);
	std::cout << "CNODE ID: " << currentNode->id << " NODE TYPE: " << static_cast<int>(currentNode->nodeType) << "CNODE AREA: " << currentNode->areaCode << std::endl;
	for (std::shared_ptr<AiGraphNode> nde : nodeQueue) {
		std::cout << "NODE ID: " << nde->id << " NODE TYPE: " << static_cast<int>(nde->nodeType) << " NODE AREA: " << nde->areaCode << std::endl;
	}
}

// A helper method to calculate the distance from the current node
float AiComponent::calcDistanceFromCurrentNode() {
	float nodeX = currentNode->position.x; // Direction, don't count Y
	float nodeZ = currentNode->position.z;
	float carX = entity->getComponent<TransformComponent>()->getGlobalPosition().x;
	float carZ = entity->getComponent<TransformComponent>()->getGlobalPosition().z;

	float dSquared = std::pow((nodeX - carX), 2) + std::pow((nodeZ - carZ), 2);
	return std::sqrt(dSquared); // POSSIBLE OPTIMIZATION, REMOVE SQRT
}

// The search state method, called every update when in SEARCH state
// Handling logic for reaching the current node the AI is approaching
// Handles logic for detecting if the car gets stuck
void AiComponent::searchState() {
	const float MINSPEED = 4.f; // minimum speed to be considered moving
	const int MAXSTUCKTIME = 90; 
	bool inBounds = inRangeOfNode(NODETHRESHOLD);

	// Checks if the car has reached the current node
	if (inBounds) {
		if (nodeQueue.size() == 1) {
			if (pickClosestParkingNode(nodeQueue[0])) {
				currentNode = nodeQueue[0];
				//nodeQueue.erase(nodeQueue.begin());
			}
			else {
				currentNode = nodeQueue[0];
				nodeQueue.erase(nodeQueue.begin());
				pickRandEntranceNode();
			}
			//nodeQueue.erase(nodeQueue.begin()); // Erase lot entrance already reached
		}
		else if(currentNode->nodeType == AiGraphNode::NodeType::PARKINGSTALL) {
			Events::CarParked.broadcast(entity);
		}
		else {
			// Reset spawn node once first node is reached
			if (currentNode->nodeType == AiGraphNode::NodeType::SPAWN) {
				currentNode->nodeTaken = false;
			}
			if (std::count(visitedAreas.begin(), visitedAreas.end(), currentNode->areaCode) == 0) {
				visitedAreas.push_back(currentNode->areaCode);
			}
			
			currentNode = nodeQueue[0];
			nodeQueue.erase(nodeQueue.begin());
		}
	}
	// Check for stuck status i.e. has not moved for too long
	else if (recoveryTimeout > MAXSTUCKTIME) {
		switchState(States::RECOVERY);
		std::cout << "RECOVERY: CNODE ID: " << currentNode->id << "CNODE AREA: " << currentNode->areaCode << std::endl;
		stuckPos = entity->getComponent<TransformComponent>()->getGlobalPosition();
		createRecoveryNode();
		recoveryTimeout = 0;
		accelReverse();
	}
	// If the AI's speed is low, count it as stuck and update the timeout count
	else if (entity->getComponent<VehicleComponent>()->
				vehicle->computeForwardSpeed() < MINSPEED) {
		recoveryTimeout++; // One more frame of not moving enough
	}
	// Normal movement for the AI, just move to the next node
	else {
		recoveryTimeout = 0;
		steerToNextNode();
	}
}

// Helper method for calculating if the AI car has reached an area around a node
bool AiComponent::inRangeOfNode(const float nodeThreshhold) {
	// COULD USE ABSOLUTE DISTANCE HERE
	float currentX = entity->getComponent<TransformComponent>()->getGlobalPosition().x;
	float currentZ = entity->getComponent<TransformComponent>()->getGlobalPosition().z;
	bool withinXBounds = currentX <= currentNode->position.x + nodeThreshhold &&
		currentX >= currentNode->position.x - nodeThreshhold;
	bool withinZBounds = currentZ <= currentNode->position.z + nodeThreshhold &&
		currentZ >= currentNode->position.z - nodeThreshhold;
	return withinXBounds && withinZBounds;
}

// Creates an extra node when in recovery mode for guidance
void  AiComponent::createRecoveryNode() {
	// Make a new temp node to direct AI
	physx::PxQuat aiCarRotation = entity->
		getComponent<TransformComponent>()->getLocalRotation();
	glm::vec3 aiForwardVec = ComputeForwardVector(aiCarRotation);
	aiForwardVec = glm::normalize(aiForwardVec);
	glm::vec3 aiSideVec = glm::vec3(-aiForwardVec.z, aiForwardVec.y, aiForwardVec.x);
	aiSideVec = aiSideVec * 8.f; // Length of 8
	glm::vec3 newNodePos = stuckPos + aiSideVec;
	std::shared_ptr<AiGraphNode> stuckNode = std::make_shared<AiGraphNode>();
	stuckNode->nodeType = AiGraphNode::NodeType::RECOVERY;
	stuckNode->position = newNodePos;
	nodeQueue.insert(nodeQueue.begin(), currentNode);
	currentNode = stuckNode;
}

// for the AI to test if it is stuck and to recover
// Reverses and turns randomly until a certain distance
// Then forward and random turns until further away from the point where stuck
void AiComponent::recoveryState() {
	const int MAXSTUCKTIME = 360; // 6 Second recovery time allowance
	float REVERSEMINIMUM = 4.f;
	float amountMoved = glm::distance(
		entity->getComponent<TransformComponent>()->getGlobalPosition(), stuckPos);

	if (recoveryTimeout > MAXSTUCKTIME) {
		//recoveryTimeout = 0;
		//accelReverse();
	}
	if (amountMoved < REVERSEMINIMUM) {
		// Randomly steer left or right
		recoveryTimeout++;
		srand(Time::now() + (double)entity->id());
		int randIntCeiling = 3; // Should give choices of 0 and 1
		int pick = rand() % randIntCeiling;
		if(pick == 0) Events::VehicleSteer.broadcast(entity, -1);
		else if (pick == 1) Events::VehicleSteer.broadcast(entity, 1);
		// Third number would be backing up straight
	}
	else {
		state = lastState;
		recoveryTimeout = 0;
		accelForwards();
	}
}

// Gets angle between goal and current forward, turns towards goal
void AiComponent::steerToNextNode() {
	const float ANGLETHRESHOLD = 3.14/10;

	physx::PxQuat aiCarRotation = entity->
		getComponent<TransformComponent>()->getLocalRotation();
	glm::vec3 aiForwardQuat = ComputeForwardVector(aiCarRotation);
	aiForwardQuat = glm::normalize(aiForwardQuat);
	// Vec between car and next node
	glm::vec3 nodesVec = currentNode->position - entity->
		getComponent<TransformComponent>()->getGlobalPosition();
	nodesVec = glm::normalize(nodesVec);

	float angle = glm::dot(aiForwardQuat, nodesVec);
	angle = angle / glm::length(aiForwardQuat);
	angle = angle / glm::length(nodesVec);
	angle = glm::acos(angle);
	glm::vec3 cross = glm::cross(aiForwardQuat, nodesVec);
	float angleFinal = glm::dot(glm::vec3(0, 1, 0), cross);
	if (angleFinal < 0) {
		float turnAmount = std::max(-1.f, (angleFinal));
		//turn left
		Events::VehicleSteer.broadcast(entity, -turnAmount);
	}
	else{
		float turnAmount = std::min(1.f, (angleFinal));
		//turn right
		Events::VehicleSteer.broadcast(entity, -turnAmount);
	}
	
}

// Method for accelerating forwards
void AiComponent::accelForwards() {
	Events::VehicleAccelerate.broadcast(entity, aiSpeed);
}
// Method for accelerating backwards i.e. reversing
void AiComponent::accelReverse() {
	Events::VehicleAccelerate.broadcast(entity, -aiSpeed);
}

// Helper method for getting the forward vector of the AI car
glm::vec3 AiComponent::ComputeForwardVector(physx::PxQuat quat) const
{
	const float x2 = 2.0f * quat.x;
	const float y2 = 2.0f * quat.y;
	const float z2 = 2.0f * quat.z;
	const float x2w = x2 * quat.w;
	const float y2w = y2 * quat.w;
	const float x2x = x2 * quat.x;
	const float z2x = z2 * quat.x;
	const float y2y = y2 * quat.y;
	const float z2y = z2 * quat.y;
	return glm::vec3(z2x + y2w, z2y - x2w, 1.0f - (x2x + y2y));
}

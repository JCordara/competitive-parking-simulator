#include "AiComponent.h"
#include "GameplaySystem.h"

AiGraphNode::AiGraphNode() {

}

AiComponent::AiComponent(Entity& parent) 
    : BaseComponent(parent)
{
	Events::AiComponentInit.broadcast(*this);
	setSpawnNode();
	std::cout << "SET THE SPAWN NODE" << std::endl;
	pickRandGoalNode();
	// forward
	Events::VehicleAccelerate.broadcast(entity, 0.3); // Something to fix
}

ComponentEnum AiComponent::getType() {
    return ComponentEnum::ai;
}

AiComponent::~AiComponent() {
    // Nothing to do here yet
}

// Expected order of executions before AiComponent updates
// AI has spawned and setCurrentNode(startNode) has been run
// Then pickRandGoalNode(globalListOfNodes, LOTENTRANCE)
// A* Will be called by the previous method and path set 
void AiComponent::update() {
	if (state == States::SEARCH) {
		searchState();
	}
	else if (state == States::PARK) {
		parkState();
	}
}

void AiComponent::aStar(std::shared_ptr<AiGraphNode> goalNode) {
	std::cout << "STARTING A STAR" << std::endl;
	//Start node is the next node in the path 
	// if the recalculated path keeps going the way the AI was going great
	// If not, then the AI can finish heading to that node and turn around
	//     can also learn to cut out the node it is going to and turn around right away
	// When a node is reached the next node becomes the current node

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
		if (Q->position == goalNode->position) break; // return Q
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
			std::sort(openList.begin(), openList.end());
		}
	}
	//exits when Q is the goal
	std::shared_ptr<AiGraphNode> tempNode = Q;
	nodeQueue.push_back(tempNode);//goal node
	while (true) {
		if (tempNode->parentNode != nullptr) {
			tempNode = tempNode->parentNode;
			nodeQueue.insert(nodeQueue.begin(), tempNode);
		}
		else {
			break;
		}
	}
	std::cout << "FINISHED A STAR" << std::endl;
	for (int i = 0; i < nodeQueue.size(); i++) {
		std::cout << "NODE ID: " << nodeQueue[i]->id << std::endl;
	}
	std::cout << "CURRENT NODE" << currentNode->id << std::endl;
}

// This is just a get distance method really
float AiComponent::getHValue(std::shared_ptr<AiGraphNode> node1, std::shared_ptr<AiGraphNode> node2) {
	float distX = std::abs(node2->position.x - node1->position.x);
	float distY = std::abs(node2->position.y - node1->position.y);
	float distZ = std::abs(node2->position.z - node1->position.z);
	// h is estimated cost of shortest path to goal node
	return distX + distY + distZ;
}

float AiComponent::getFValue(std::shared_ptr<AiGraphNode> node) {
	// f is distance between start, node and goal node
	return node->g + node->h;
}

// Set the next node for the AI to go to
void AiComponent::setSpawnNode() {
	for each (std::shared_ptr<AiGraphNode> node in gameplaySystem->aiGlobalNodes) {
		if (node->nodeType == AiGraphNode::NodeType::SPAWN && !(node->spawnTaken)) {
			currentNode = node;
			node->spawnTaken = true;
			return;
		}
	}
}

void AiComponent::switchState(States newState) {
	state = newState;
}

void AiComponent::searchState() {
	const float NODETHRESHOLD = 2.5; // How close to
	// COULD USE A PHYSX TRIGGERBOX INSTEAD HERE
	float currentX = entity.getComponent<TransformComponent>()->getGlobalPosition().x;
	float currentZ = entity.getComponent<TransformComponent>()->getGlobalPosition().z;
	bool withinXBounds =	currentX <= currentNode->position.x + NODETHRESHOLD &&
							currentX >= currentNode->position.x - NODETHRESHOLD;
	bool withinZBounds =	currentZ <= currentNode->position.z + NODETHRESHOLD &&
							currentZ >= currentNode->position.z - NODETHRESHOLD;
	if (withinXBounds && withinZBounds) {
		if (currentNode->nodeType == AiGraphNode::NodeType::LOTENTRANCE) {
			pickClosestParkingNode(currentNode);
			state = States::PARK; // Reached entrance to parking lot
		}
		else {
			currentNode = nodeQueue[0];
			std::cout << "CURRENT NODE" << currentNode->id << std::endl;
			nodeQueue.erase(nodeQueue.begin());
		}
	}
	moveToNextNode();
}

// Finds the closest parking lot node
// Assumes node doesnt get taken
void AiComponent::pickClosestParkingNode(std::shared_ptr<AiGraphNode> startNode) {
	std::vector<std::shared_ptr<AiGraphNode>> parkingLot;
	std::vector<std::shared_ptr<AiGraphNode>> visited;
	for each (std::shared_ptr<AiGraphNode> node in startNode->neighbours) {
		if (node->nodeType == AiGraphNode::NodeType::INNERLOT) {
			parkingLot.push_back(node);
		}
		else if (node->nodeType == AiGraphNode::NodeType::PARKINGSTALL) {
			currentNode = node;
			return;
		}
	}
	visited.push_back(startNode);

	for (int i = 0; i < parkingLot.size(); i++) {
		for each (std::shared_ptr<AiGraphNode> neighbour in parkingLot[i]->neighbours) {
			if (parkingLot[i]->nodeType == AiGraphNode::NodeType::INNERLOT) {
				if (std::count(visited.begin(), visited.end(), neighbour) == 0) {
					parkingLot.push_back(neighbour);
				}
			}
			else if (parkingLot[i]->nodeType == AiGraphNode::NodeType::PARKINGSTALL) {
				nodeQueue.clear(); // clear path for A*
				aStar(parkingLot[i]); // Sets path
				return;
			}
		}
		//::cout << "ERASING NODE: " << parkingLot[i]->id << std::endl;
		//parkingLot.erase(parkingLot.begin() + i);
		//std::remove(parkingLot.begin(), parkingLot.end(), node);
		visited.push_back(parkingLot[i]);
	}
}

void AiComponent::pickRandGoalNode() {
	// Randomly pick a node from the entrances to parking lots
	std::vector<std::shared_ptr<AiGraphNode>> nodes;
	for each (std::shared_ptr<AiGraphNode> node in gameplaySystem->aiGlobalNodes) {
		if (node->nodeType == AiGraphNode::NodeType::LOTENTRANCE) {
			nodes.push_back(node);
		}
	}
	if (nodes.size() == 0) return;
	int randIntCeiling = nodes.size();
	std::srand(Time::now());
	// Should give number between 0 and vector.szie()-1
	int pick = rand() % randIntCeiling;
	aStar(nodes[pick]);
}

void AiComponent::parkState() {
	const float NODETHRESHOLD = 1.5; // How close to
	// COULD USE A PHYSX TRIGGERBOX INSTEAD HERE
	float currentX = entity.getComponent<TransformComponent>()->getGlobalPosition().x;
	float currentZ = entity.getComponent<TransformComponent>()->getGlobalPosition().z;
	bool withinXBounds =	currentX <= currentNode->position.x + NODETHRESHOLD &&
							currentX >= currentNode->position.x - NODETHRESHOLD;
	bool withinZBounds =	currentZ <= currentNode->position.z + NODETHRESHOLD &&
							currentZ >= currentNode->position.z - NODETHRESHOLD;
	if (withinXBounds && withinZBounds) {
		if (currentNode->nodeType == AiGraphNode::NodeType::PARKINGSTALL) {
			Events::VehicleBrake.broadcast(entity, 1.f);
		}
		else {
			currentNode = nodeQueue[0];
			std::cout << "CURRENT NODE" << currentNode->id << std::endl;
			nodeQueue.erase(nodeQueue.begin());
		}
	}
	moveToNextNode();
}

// Gets angle between goal and current forward, turns a bit in that direction nad goes forward.
void AiComponent::moveToNextNode() {
	const float ANGLETHRESHOLD = 3.14/10;
	// Less sure this method works
	glm::mat4 aiCarTransform = entity.getComponent<TransformComponent>()->getGlobalMatrix();
	glm::vec3 aiForward = glm::vec3(aiCarTransform[2][0], aiCarTransform[2][1], aiCarTransform[2][2]);
	// This method might work better
	physx::PxQuat aiCarRotation = entity.getComponent<TransformComponent>()->getLocalRotation();
	glm::vec3 aiForwardQuat = ComputeForwardVector(aiCarRotation);
	aiForwardQuat = glm::normalize(aiForwardQuat);
	// Vec between car and next node
	glm::vec3 nodesVec = currentNode->position - entity.getComponent<TransformComponent>()->getGlobalPosition();
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

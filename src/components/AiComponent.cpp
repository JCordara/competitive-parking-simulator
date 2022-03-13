#include "AiComponent.h"
#include "GameplaySystem.h"

AiGraphNode::AiGraphNode() {

}

AiComponent::AiComponent(shared_ptr<Entity> parent) 
    : BaseComponent(parent)
{
	Events::AiComponentInit.broadcast(*this);
	setSpawnNode();
	std::cout << "SET THE SPAWN NODE" << std::endl;
	pickRandGoalNode();
	// forward
	Events::VehicleAccelerate.broadcast(entity, 0.65); 
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
	else if (state == States::RECOVERY) {
		recoveryState();
	}
}

void AiComponent::aStar(std::shared_ptr<AiGraphNode> goalNode) {
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
		if (node->nodeType == AiGraphNode::NodeType::SPAWN && !(node->nodeTaken)) {
			currentNode = node;
			node->nodeTaken = true;
			node->spawnAiComponent = entity->shared_from_this();
			entity->getComponent<TransformComponent>()->setLocalPosition(node->position);
			return;
		}
	}
}

void AiComponent::switchState(States newState) {
	state = newState;
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
	std::srand(Time::now() + (double)entity->id()); // Get AI picking differently
	// Should give number between 0 and vector.szie()-1
	int pick = rand() % randIntCeiling;
	aStar(nodes[pick]);
}

float AiComponent::calcDistanceFromCurrentNode() {
	float nodeX = currentNode->position.x; // Direction, don't count Y
	float nodeZ = currentNode->position.z;
	float carX = entity->getComponent<TransformComponent>()->getGlobalPosition().x;
	float carZ = entity->getComponent<TransformComponent>()->getGlobalPosition().z;

	float dSquared = std::pow((nodeX - carX), 2) + std::pow((nodeZ - carZ), 2);
	return std::sqrt(dSquared); // POSSIBLE OPTIMIZATION, REMOVE SQRT
}

void AiComponent::searchState() {
	const float NODETHRESHOLD = 2.5; // How close to
	const float MINSPEED = 4.f; // minimum speed to be considered moving
	const int MAXSTUCKTIME = 90; 
	// COULD USE ABSOLUTE DISTANCE HERE
	float currentX = entity->getComponent<TransformComponent>()->getGlobalPosition().x;
	float currentZ = entity->getComponent<TransformComponent>()->getGlobalPosition().z;
	bool withinXBounds = currentX <= currentNode->position.x + NODETHRESHOLD &&
		currentX >= currentNode->position.x - NODETHRESHOLD;
	bool withinZBounds = currentZ <= currentNode->position.z + NODETHRESHOLD &&
		currentZ >= currentNode->position.z - NODETHRESHOLD;
	if (withinXBounds && withinZBounds) {
		if (currentNode->nodeType == AiGraphNode::NodeType::LOTENTRANCE) {
			pickClosestParkingNode(currentNode);
			state = States::PARK; // Reached entrance to parking lot
			lastState = States::SEARCH;
		}
		else {
			currentNode = nodeQueue[0];
			std::cout << "CURRENT NODE" << currentNode->id << std::endl;
			nodeQueue.erase(nodeQueue.begin());
		}
	}
	else if (recoveryTimeout > MAXSTUCKTIME) {

		lastState = States::SEARCH;
		state = States::RECOVERY;
		stuckPos = entity->getComponent<TransformComponent>()->getGlobalPosition();
		physx::PxQuat aiCarRotation = entity->
			getComponent<TransformComponent>()->getLocalRotation();
		glm::vec3 aiForwardVec = ComputeForwardVector(aiCarRotation);
		aiForwardVec = glm::normalize(aiForwardVec);
		glm::vec3 aiSideVec = glm::vec3(-aiForwardVec.y, aiForwardVec.x, aiForwardVec.z);
		aiSideVec = aiSideVec * 8.f; // Length of 8
		glm::vec3 newNodePos = stuckPos + aiSideVec;
		std::shared_ptr<AiGraphNode> stuckNode = std::make_shared<AiGraphNode>();
		stuckNode->nodeType = AiGraphNode::NodeType::RECOVERY;
		stuckNode->position = newNodePos;
		nodeQueue.insert(nodeQueue.begin(), currentNode);
		currentNode = stuckNode;
		recoveryTimeout = 0;
		Events::VehicleAccelerate.broadcast(entity, -0.65); // reverse
	}
	else if (entity->getComponent<VehicleComponent>()->vehicle->computeForwardSpeed() < MINSPEED) {
		recoveryTimeout++; // One more frame of not moving enough
	}
	else {
		recoveryTimeout = 0;
		moveToNextNode();
	}
}

void AiComponent::parkState() {
	const float NODETHRESHOLD = 1.5; // How close to
	const float MINSPEED = 4.f; // minimum speed to be considered moving
	const int MAXSTUCKTIME = 90;
	// COULD USE ABSOLUTE DISTANCE HERE
	float currentX = entity->getComponent<TransformComponent>()->getGlobalPosition().x;
	float currentZ = entity->getComponent<TransformComponent>()->getGlobalPosition().z;
	bool withinXBounds =	currentX <= currentNode->position.x + NODETHRESHOLD &&
							currentX >= currentNode->position.x - NODETHRESHOLD;
	bool withinZBounds =	currentZ <= currentNode->position.z + NODETHRESHOLD &&
							currentZ >= currentNode->position.z - NODETHRESHOLD;
	if (withinXBounds && withinZBounds) {
		if (currentNode->nodeType == AiGraphNode::NodeType::PARKINGSTALL) {
			//Events::VehicleBrake.broadcast(entity, 1.f);
			Events::CarParked.broadcast(entity);
		}
		else {
			currentNode = nodeQueue[0];
			std::cout << "CURRENT NODE" << currentNode->id << std::endl;
			nodeQueue.erase(nodeQueue.begin());
		}
	}
	else if (recoveryTimeout > MAXSTUCKTIME) {

		lastState = States::PARK;
		state = States::RECOVERY;
		stuckPos = entity->getComponent<TransformComponent>()->getGlobalPosition();
		physx::PxQuat aiCarRotation = entity->
			getComponent<TransformComponent>()->getLocalRotation();
		glm::vec3 aiForwardVec = ComputeForwardVector(aiCarRotation);
		aiForwardVec = glm::normalize(aiForwardVec);
		glm::vec3 aiSideVec = glm::vec3(-aiForwardVec.y, aiForwardVec.x, aiForwardVec.z);
		aiSideVec = aiSideVec * 8.f; // Length of 8
		glm::vec3 newNodePos = stuckPos + aiSideVec;
		std::shared_ptr<AiGraphNode> stuckNode = std::make_shared<AiGraphNode>();
		stuckNode->nodeType = AiGraphNode::NodeType::RECOVERY;
		stuckNode->position = newNodePos;
		nodeQueue.insert(nodeQueue.begin(), currentNode);
		currentNode = stuckNode;
		recoveryTimeout = 0;
		Events::VehicleAccelerate.broadcast(entity, -0.65); // reverse
	}
	else if (entity->getComponent<VehicleComponent>()->vehicle->computeForwardSpeed() < MINSPEED) {
		recoveryTimeout++; // One more frame of not moving enough
	}
	else {
		recoveryTimeout = 0;
		moveToNextNode();
	}
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
		//Events::VehicleAccelerate.broadcast(entity, -0.65); // reverse
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
//	else if (amountMoved < FORWARDMINIMUM) {
//		Events::VehicleAccelerate.broadcast(entity, 0.65);
//		recoveryTimeout++; // In this state means the car is not stuck
//	}
	else {
		// Make a new temp node to direct AI
		
		state = lastState;
		//lastState = States::RECOVERY;
		recoveryTimeout = 0;
		Events::VehicleAccelerate.broadcast(entity, 0.65); // reverse
	}
}

// Gets angle between goal and current forward, turns a bit in that direction nad goes forward.
void AiComponent::moveToNextNode() {
	const float ANGLETHRESHOLD = 3.14/10;

	physx::PxQuat aiCarRotation = entity->getComponent<TransformComponent>()->getLocalRotation();
	glm::vec3 aiForwardQuat = ComputeForwardVector(aiCarRotation);
	aiForwardQuat = glm::normalize(aiForwardQuat);
	// Vec between car and next node
	glm::vec3 nodesVec = currentNode->position - entity->getComponent<TransformComponent>()->getGlobalPosition();
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

#include "AiComponent.h"

AiComponent::AiComponent(Entity& parent) 
    : BaseComponent(parent)
{

}

ComponentEnum AiComponent::getType() {
    return ComponentEnum::ai;
}

AiComponent::~AiComponent() {
    // Nothing to do here yet
}

void AiComponent::aStar() {
	//Start node is the next node in the path 
	// if the recalculated path keeps going the way the AI was going great
	// If not, then the AI can finish heading to that node and turn around
	//     can also learn to cut out the node it is going to and turn around right away
	// When a node is reached the next node becomes the current node



	std::vector<std::shared_ptr<AiGraphNode>> openList;
	std::vector<std::shared_ptr<AiGraphNode>> closedList;
	std::shared_ptr<AiGraphNode> goalNode;

	// Initialize open list with starting node
	currentNode->parent = nullptr;
	currentNode->g = 0;
	openList.push_back(currentNode);

	std::shared_ptr<AiGraphNode> Q = currentNode; // just to start
	while (true) {
		if (openList.size() == 0) break;
		// Find next node closest to goal
		for each (std::shared_ptr<AiGraphNode> openNode in openList) {
			if (getFValue(openNode) < getFValue(Q)) {
				Q = openNode;
			}
		}
		std::remove(openList.begin(), openList.end(), Q);
		if (Q->position == goalNode->position) break; // return Q
		closedList.push_back(Q);
		for each (std::shared_ptr<AiGraphNode> neighbor in Q->neighbours){
			std::shared_ptr<AiGraphNode> temp = Q;
			neighbor->parent = temp;
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
			for each (std::shared_ptr<AiGraphNode> openNode in closedList) {
				if (openNode->position == neighbor->position) {
					if (getFValue(openNode) <= getFValue(neighbor)) {
						skip = true;
					}
				}
			}
			if (!skip) openList.push_back(neighbor);
			std::sort(openList.begin(), openList.end());
		}
	}
	//exits when Q is the goal
	std::shared_ptr<AiGraphNode> tempNode = Q;
	nodeQueue.push_back(tempNode);//goal node
	while (true) {
		tempNode = tempNode->parent;
		nodeQueue.push_back(tempNode);//goal node
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
void AiComponent::setCurrentNode(std::shared_ptr<AiGraphNode> startNode) {
	currentNode = startNode;
}

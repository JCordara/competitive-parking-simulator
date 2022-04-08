#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H

#include "Components.h"

#include <algorithm>
#include <TimeInfo.h>
#include <PhysX/PxPhysicsAPI.h>


class GameplaySystem;


class AiGraphNode {
public:
	enum class NodeType {
		TRAVERSAL = 1,
		OUTERROAD, // TODO Remove
		LOTENTRANCE,
		INNERLOT, // TODO Remove
		PARKINGSTALL,
		INTERSECTION, // TODO Remove
		SPAWN,
		RECOVERY
	};
	AiGraphNode();
	// Sorting operator for f values
	bool operator< (const std::shared_ptr<AiGraphNode>& other) const {
		return g+h < other->g + other->h;
	}
	std::vector<std::shared_ptr<AiGraphNode>> neighbours;
	glm::vec3 position = glm::vec3(1,1,1);
	std::shared_ptr<AiGraphNode> parentNode; // For A* algorithm
	float g = 0; // For A* algorithm
	float h = 0; // For A* algorithm
	NodeType nodeType = NodeType::SPAWN;
	//std::shared_ptr<Entity> spawnAiComponent = nullptr; // Only for Spawn nodes
	bool nodeTaken = false;
	int id = 0;
	int areaCode;
	float nodeSpeed = 0.05f; // When car hits node, tells to slow down or not
	float nodeThreshold = 4.5; // A threshold for how far away the car has to be to the node
};

class AiComponent : public BaseComponent {
public:

	enum class States {
		SEARCH = 1,
		ATTACK,
		RECOVERY,
		RESET,
		PARKED
	};

	float aiSpeed = 0.35f;
	const float NODETHRESHOLD = 4.5f;
	float sightFrontConeWidth;
	float sightFrontConeHeight;
	float sightBackConeWidth;
	float sightBackConeHeight;
	// Node the AI car currently belongs to
	std::shared_ptr<AiGraphNode> currentNode = std::make_shared<AiGraphNode>();

    AiComponent(shared_ptr<Entity> parent);
	AiComponent(shared_ptr<Entity> parent, std::vector<glm::vec3> nodeLocs);
	void setParkingNode(std::vector<glm::vec3> nodeLocs);
	void pickParkingNode();
    ~AiComponent();
    static ComponentEnum getType();
    void update();
	void switchState(States newState);
	void setGameplaySystem(shared_ptr<GameplaySystem> system)
		{ gameplaySystem = system; }
	void setSpawnNode();
	void pickRandEntranceNode();
	void resetAi();

private:

	shared_ptr<GameplaySystem> gameplaySystem;

	States state = States::SEARCH; // Default search
	States lastState = States::RESET; // Default
	glm::vec3 stuckPos = glm::vec3(); // Used for original stuck position
	int recoveryTimeout = 0; // Count of frames where the vehicle is not moving enough
	std::vector<Entity> carQueue;
	std::vector<std::shared_ptr<AiGraphNode>> nodeQueue;
	std::vector<std::shared_ptr<AiGraphNode>> emptyParkingNodes;
	std::vector<int> visitedAreas;
	std::unordered_map<int, std::vector<int>> areaMap;

	void aStar(std::shared_ptr<AiGraphNode> goalNode);
	float getFValue(std::shared_ptr<AiGraphNode> node);
	float getHValue(std::shared_ptr<AiGraphNode> node,
					std::shared_ptr<AiGraphNode> goalNode);
	void searchState();
	void recoveryState();
	float calcDistanceFromCurrentNode();
	bool inRangeOfNode(float nodeThreshhold);
	bool pickClosestParkingNode(std::shared_ptr<AiGraphNode> startNode);
	void createRecoveryNode();
	void steerToNextNode();
	void accelForwards();
	void accelReverse();
	glm::vec3 ComputeForwardVector(physx::PxQuat quat) const;
	float calcAngle2Vec();
	void setupAreaMap();
};

#endif // AI_COMPONENT_H

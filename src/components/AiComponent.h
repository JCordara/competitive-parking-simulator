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
		OUTERROAD = 1,
		LOTENTRANCE,
		INNERLOT,
		PARKINGSTALL,
		INTERSECTION,
		SPAWN
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
	std::shared_ptr<Entity> spawnAiComponent = nullptr; // Only for Spawn nodes
	bool nodeTaken = false;
	int id = 0;
};

class AiComponent : public BaseComponent {
public:

	enum class States {
		SEARCH = 1,
		PARK,
		ATTACK
	};
    AiComponent(Entity& parent);
    ~AiComponent();
    static ComponentEnum getType();

    void update();
	void switchState(States newState);

	void setGameplaySystem(shared_ptr<GameplaySystem> system) { 
		gameplaySystem = system; 
	}

private:

	shared_ptr<GameplaySystem> gameplaySystem;

	States state = States::SEARCH; // Default search
	std::vector<Entity> carQueue;
	std::vector<std::shared_ptr<AiGraphNode>> nodeQueue;
	// std::vector<GraphNode> graph;
	std::shared_ptr<AiGraphNode> currentNode = std::make_shared<AiGraphNode>(); // Node the AI car currently belongs to

	void setSpawnNode();
	void pickRandGoalNode();
	void aStar(std::shared_ptr<AiGraphNode> goalNode);
	float getFValue(std::shared_ptr<AiGraphNode> node);
	float getHValue(std::shared_ptr<AiGraphNode> node,
					std::shared_ptr<AiGraphNode> goalNode);
	void searchState();
	void parkState();
	void pickClosestParkingNode(std::shared_ptr<AiGraphNode> startNode);
	void moveToNextNode();
	glm::vec3 ComputeForwardVector(physx::PxQuat quat) const;
	float calcAngle2Vec();


    float sightFrontConeWidth;
    float sightFrontConeHeight;
    float sightBackConeWidth;
    float sightBackConeHeight;

};

#endif // AI_COMPONENT_H

#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H

#include "Components.h"

class AiGraphNode {
public:
	// Sorting operator for f values
	bool operator< (const AiGraphNode& other) const {
		return g+h < other.g + other.h;
	}
	std::vector<std::shared_ptr<AiGraphNode>> neighbours;
	glm::vec3 position;
	std::shared_ptr<AiGraphNode> parent; // For A* algorithm
	float g; // For A* algorithm
	float h; // For A* algorithm
};

class AiComponent : public BaseComponent {
public:
    AiComponent(Entity& parent);
    ~AiComponent();
    static ComponentEnum getType();

    void update();
	void setCurrentNode(std::shared_ptr<AiGraphNode> startNode);

private:
	void aStar();
	float getFValue(std::shared_ptr<AiGraphNode> node);
	float getHValue(std::shared_ptr<AiGraphNode> node, std::shared_ptr<AiGraphNode> goalNode);
    std::vector<Entity> carQueue;
    std::vector<std::shared_ptr<AiGraphNode>> nodeQueue;
	// std::vector<GraphNode> graph;
	std::shared_ptr<AiGraphNode> currentNode; // Node the AI car currently belings to

    float sightFrontConeWidth;
    float sightFrontConeHeight;
    float sightBackConeWidth;
    float sightBackConeHeight;

};

#endif // AI_COMPONENT_H

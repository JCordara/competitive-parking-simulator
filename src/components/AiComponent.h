#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H

#include "Components.h"


struct AiGraphNode {
	std::vector<std::shared_ptr<AiGraphNode>> neighbours;
	glm::vec3 position;
};

class AiComponent : public BaseComponent {
public:
    AiComponent(Entity& parent);
    ~AiComponent();
    static ComponentEnum getType();

    // void update();
	void setMap(std::vector<std::vector<int>>);

private:
	void aStar();

    std::vector<Entity> carQueue;
    // std::vector<Something> nodeQueue;
	//std::vector<GraphNodes> graph;

    float sightFrontConeWidth;
    float sightFrontConeHeight;
    float sightBackConeWidth;
    float sightBackConeHeight;

};

#endif // AI_COMPONENT_H

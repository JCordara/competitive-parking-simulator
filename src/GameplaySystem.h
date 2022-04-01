#ifndef GAMEPLAY_SYSTEM_H
#define GAMEPLAY_SYSTEM_H

#include "GameSystem.h"

extern std::unordered_map<unsigned int, int> scores;
extern unsigned int playerId;
extern std::vector<unsigned int> aiList;

class GameplaySystem : public GameSystem {
public:

    friend class AiComponent;

    /* Prepare framework */
    GameplaySystem(std::shared_ptr<Scene> scene);

	void defineMap(
		std::vector<std::shared_ptr<AiGraphNode>> graph,
		std::vector<instancedTransformation> parkingSpots,
		std::vector<instancedTransformation> emptyParkingSpots
	);

	void setRoundEmptyParkingSpots(std::vector<instancedTransformation> emptyParkingSpots);
    void update();
    ~GameplaySystem();
	void setupAiNodes();
	std::vector<std::shared_ptr<AiGraphNode>> aiGlobalNodes;
	std::vector<std::shared_ptr<AiGraphNode>> area950Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area951Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area952Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area953Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area954Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area955Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area956Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area957Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area958Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area959Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area960Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area961Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area962Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area963Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area964Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area965Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area966Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area967Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area968Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area969Nodes;
	std::vector<std::shared_ptr<AiGraphNode>> area970Nodes;
    void registerAiComponent(AiComponent& component);
	void registerCarParked(shared_ptr<Entity> entity);
	
	void setPlayerId(unsigned int);
	
	void addAiId(unsigned int);
	
	void resetPlayer();
	void resetAi(unsigned int);
	bool gameWon();
	void addAINode(std::string nodeType, int nodeAreaCode, glm::vec3 nodePos);
	void setNodeType(std::string nodeType, std::shared_ptr<AiGraphNode> aiNode);
	int lastNodeID = 0;
	void testPrintAINodes();
	void setAINodeNeighbors(std::shared_ptr<AiGraphNode> aiNode1,
		int id,
		std::vector<std::shared_ptr<AiGraphNode>> list);
	
	std::vector<std::shared_ptr<AiGraphNode>> getAreaNodes(int nodeAreaCode);

private:
    std::shared_ptr<Scene> scene;

	double lastUpdateTime;

};

#endif // GAMEPLAY_SYSTEM_H

#ifndef GAMEPLAY_SYSTEM_H
#define GAMEPLAY_SYSTEM_H
#include "GameSystem.h"
#include "Event.h"

extern std::vector<unsigned int> aiList;

class GameplaySystem : public GameSystem {
public:

    friend class AiComponent;

    /* Prepare framework */
    GameplaySystem(std::shared_ptr<Scene> scene);
	//Only Called once
	void defineMap(
		std::string graph,
		std::vector<instancedTransformation> parkingSpots,
		std::vector<instancedTransformation> emptyParkingSpots
	);
	//When this is called, the map will resent with numberOfParkingSpots empty for play
	void resetMapWithNumberOfEmptyParkingSpaces(unsigned int numberOfParkingSpots);
	void cleanMap();

	//Game Events
	void setupNewGame();
	void setupNewRound();
	void cleanUpGame();

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

	std::shared_ptr<AiGraphNode> addAINode(std::string nodeType, int id, glm::vec3 nodePos);
	void readAiGraph(string filepath);
	void setNeigbours(std::shared_ptr<AiGraphNode> nodePrime, std::vector<int> nodeNeighbours);
	std::shared_ptr<AiGraphNode> retrieveNode(int inId);
	void setNodeType(std::string nodeType, std::shared_ptr<AiGraphNode> aiNode);
	int lastNodeID = 0;
	void testPrintAINodes();
	void setAINodeNeighbors(std::shared_ptr<AiGraphNode> aiNode1,
		int id,
		std::vector<std::shared_ptr<AiGraphNode>> list);
	
	std::vector<std::shared_ptr<AiGraphNode>> getAreaNodes(int nodeAreaCode);

private:
	// --- Gui Control unit --- //
	enum class GameState {
		MainMenu,
		Playing,
		RoundEnd,
		GameEnd
	};
	bool updateMenu;
	GameState gamestate;
	// --- ---------------- --- //
    std::shared_ptr<Scene> scene;
	double lastUpdateTime;

	std::vector<instancedTransformation> possibleParkingSpots;
	std::vector<bool> parking;
	std::vector<int> scores;
	unsigned int nextAI_ID;
	unsigned int startingAi_number;
	unsigned int currentAi_number;
};

bool prefix(const string& prefix, const string& base);

#endif // GAMEPLAY_SYSTEM_H

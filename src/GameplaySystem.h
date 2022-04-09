#ifndef GAMEPLAY_SYSTEM_H
#define GAMEPLAY_SYSTEM_H
#include "GameSystem.h"
#include "Event.h"
#include <vector>

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
	void removeBottomAI(unsigned int num);
	void deleteQueue();

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
	void registerCarParked(weak_ptr<Entity> VehicleEntity, weak_ptr<Entity> TriggerEntity);
	void setNumberOfAI(int num);
	std::shared_ptr<AiGraphNode> addAINode(const std::string nodeType, int id, glm::vec3 nodePos);
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


	//Not AI
	unsigned int getStartingAi_number() { return startingAi_number; }
private:
	// --- Gui Control unit --- //
	enum class GameState {
		MainMenu,
		Options,
		Playing,
		RoundEnd,
		GameEnd
	};
	GameState gamestate;
	void setGameState(GameState gs) {
		gamestate = gs;
	}
	// --- ---------------- --- //
    std::shared_ptr<Scene> scene;
	double lastUpdateTime;

	/* --- Parking Data --- */
	std::vector<instancedTransformation>	possibleParkingSpots;
	std::vector<bool>						parking;
	/* --- Player Scores --- */
	std::unordered_map<int, int> scores;
	unsigned int nextAI_ID;
	unsigned int startingAi_number;
	unsigned int currentAi_number;

	vector<weak_ptr<Entity>> entitiesToDelete;

};

bool prefix(const string& prefix, const string& base);

#endif // GAMEPLAY_SYSTEM_H

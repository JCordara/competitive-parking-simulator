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

    void update();
    ~GameplaySystem();
	void setupAiNodes();
	std::vector<std::shared_ptr<AiGraphNode>> aiGlobalNodes;
    void registerAiComponent(AiComponent& component);
	void registerCarParked(Entity& entity);
	
	void setPlayerId(unsigned int);
	
	void addAiId(unsigned int);
	
	void resetPlayer();
	void resetAi(unsigned int);
	bool gameWon();
	


private:
    std::shared_ptr<Scene> scene;

};

#endif // GAMEPLAY_SYSTEM_H

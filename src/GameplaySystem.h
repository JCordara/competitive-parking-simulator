#ifndef GAMEPLAY_SYSTEM_H
#define GAMEPLAY_SYSTEM_H

#include "GameSystem.h"

class GameplaySystem : public GameSystem {
public:

    /* Prepare framework */
    GameplaySystem(std::shared_ptr<Scene> scene);

    void update();
    ~GameplaySystem();
	void setupAiNodes();
	std::vector<std::shared_ptr<AiGraphNode>> aiGlobalNodes;

private:
    std::shared_ptr<Scene> scene;

};

#endif // GAMEPLAY_SYSTEM_H

#ifndef GAMEPLAY_SYSTEM_H
#define GAMEPLAY_SYSTEM_H

#include "GameSystem.h"

class GameplaySystem : public GameSystem {
public:

    friend class AiComponent;

    /* Prepare framework */
    GameplaySystem(std::shared_ptr<Scene> scene);

    void update();
    ~GameplaySystem();

    void registerAiComponent(AiComponent& component);

private:
    std::shared_ptr<Scene> scene;

};

#endif // GAMEPLAY_SYSTEM_H

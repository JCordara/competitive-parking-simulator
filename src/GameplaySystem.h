#ifndef GAMEPLAY_SYSTEM_H
#define GAMEPLAY_SYSTEM_H

#include "GameSystem.h"

#include "Framework.h"

class GameplaySystem : public GameSystem {
public:

    /* Prepare framework */
    GameplaySystem(std::shared_ptr<Scene> scene);

    void update();
    ~GameplaySystem();

private:
    std::shared_ptr<Scene> scene;

};

#endif // GAMEPLAY_SYSTEM_H

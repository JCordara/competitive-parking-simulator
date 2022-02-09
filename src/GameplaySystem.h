#ifndef GAMEPLAY_SYSTEM_H
#define GAMEPLAY_SYSTEM_H

#include "GameSystem.h"


class GameplaySystem : public GameSystem {
public:

    /* Prepare framework */
    GameplaySystem(
        std::shared_ptr<Scene> scene,
        std::shared_ptr<GameEventManager> event,
        std::shared_ptr<AudioManager> audio
    );

    void update();
    ~GameplaySystem();
private:
    std::shared_ptr<Scene>             scene;
	std::shared_ptr<GameEventManager>  eventManager;
	std::shared_ptr<AudioManager>      audioManager;
};

#endif // GAMEPLAY_SYSTEM_H

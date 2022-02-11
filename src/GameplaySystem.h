#ifndef GAMEPLAY_SYSTEM_H
#define GAMEPLAY_SYSTEM_H

#include "GameSystem.h"

#include "Framework.h"

class GameplaySystem : public GameSystem {
public:

    /* Prepare framework */
    GameplaySystem(
        std::shared_ptr<Scene> scene,
        std::shared_ptr<GameEventManager> event,
        std::shared_ptr<AudioManager> audio,
        GameObject& c
    );

    void update();
    ~GameplaySystem();

private:
    std::shared_ptr<Scene>             scene;
	std::shared_ptr<GameEventManager>  eventManager;
	std::shared_ptr<AudioManager>      audioManager;

    // do u smell spaghetti
    GameObject& car;
};

#endif // GAMEPLAY_SYSTEM_H

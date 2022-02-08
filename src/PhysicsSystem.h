#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "GameSystem.h"


class PhysicsSystem : public GameSystem {
public:

    /* Prepare framework */
    PhysicsSystem(
        std::shared_ptr<Scene> scene,
        std::shared_ptr<GameEventManager> event,
        std::shared_ptr<AudioManager> audio
    );

    void update();
    ~PhysicsSystem();
    
private:
    std::shared_ptr<Scene>             scene;
	std::shared_ptr<GameEventManager>  eventManager;
	std::shared_ptr<AudioManager>      audioManager;
};


#endif // PHYSICS_SYSTEM_H

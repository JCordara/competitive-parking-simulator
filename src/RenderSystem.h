#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "GameSystem.h"
#include "Window.h"

class RenderSystem : public GameSystem {
public:

    /* Prepare framework */
    RenderSystem(
        std::shared_ptr<Scene> scene,
        std::shared_ptr<AudioManager> audio,
        std::shared_ptr<Window> window
    );

    void update();
    ~RenderSystem();
    
private:
    std::shared_ptr<Scene>             scene;
	std::shared_ptr<AudioManager>      audioManager;
    std::shared_ptr<Window>            window;
};


#endif // RENDER_SYSTEM_H

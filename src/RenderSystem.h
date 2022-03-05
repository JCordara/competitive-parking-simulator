#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "GameSystem.h"
#include "Window.h"
#include <Renderers.h>
#include <Entity.h>
#include <GUI.h>

class RenderSystem : public GameSystem {
public:

    /* Prepare framework */
    RenderSystem(
        std::shared_ptr<Scene> scene,
        std::shared_ptr<Window> window
    );

    void update();
    ~RenderSystem();
    
private:
    std::shared_ptr<Scene>             scene;
    std::shared_ptr<Window>            window;

	std::shared_ptr<GameRenderPipeline> renderPipeline;
	std::shared_ptr<GUI> gui;

};


#endif // RENDER_SYSTEM_H

#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "GameSystem.h"
#include "Window.h"
#include <Renderers.h>
#include <Entity.h>
#include <GuiScene.h>

class RenderSystem : public GameSystem {
public:

    /* Prepare framework */
    RenderSystem(
        std::shared_ptr<Scene> scene,
        std::shared_ptr<GuiScene> gui,
        std::shared_ptr<Window> window
    );

    void update();

    void changeScene(shared_ptr<Scene> newScene);
    void changeGui(shared_ptr<GuiScene> newGui);
	void setPlaying(bool b) { playing = b; }

    ~RenderSystem();
    
private:

    std::shared_ptr<Scene>    scene;
    std::shared_ptr<Window>   window;
	std::shared_ptr<GuiScene> guiScene;

	std::shared_ptr<GameRenderPipeline> renderPipeline;

	bool playing = false;

};


#endif // RENDER_SYSTEM_H

glm::vec3 VectorPlaneProjection(glm::vec3 vec, glm::vec3 normal);
glm::vec3 VectorVectorProjection(glm::vec3 vec, glm::vec3 vec0);
glm::mat4 getLocalToGlobalTransformation(sp<Entity> e);

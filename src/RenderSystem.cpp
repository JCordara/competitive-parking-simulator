#include "RenderSystem.h"

RenderSystem::RenderSystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<GameEventManager> event,
    std::shared_ptr<AudioManager> audio,
    std::shared_ptr<Window> window)
    : scene(scene)
    , eventManager(event)
    , audioManager(audio)
    , window(window)
{
    /* Rendering system initialization */
}

void RenderSystem::update() {
    /* Rendering per-frame updates */
}

RenderSystem::~RenderSystem() {
    
}

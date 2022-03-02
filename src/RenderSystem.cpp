#include "RenderSystem.h"
#include <GUI.h>

RenderSystem::RenderSystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<AudioManager> audio,
    std::shared_ptr<Window> window)
    : scene(scene)
    , audioManager(audio)
    , window(window)
{
    /* Rendering system initialization */
	renderPipeline = std::make_shared<GameRenderPipeline>();
	gui = std::make_shared<GUI>();
}

void RenderSystem::update() {
	//Setting up window
	renderPipeline->setWindowDimentions(window->getWidth(), window->getHeight());

	//Attach all entity objects to render to render
	for (auto e = scene->; e != scene->end(); e++) {//Willl change, need to do tree traversal
		auto lightingComponent = e->getComponent<LightingComponent>();
		auto transformComponent = e->getComponent<TransformComponent>();
		auto cameraComponent = e->getComponent<CameraComponent>();
		auto modelComponent = e->getComponent<ModelComponent>();
		auto rendererComponent = e->getComponent<RendererComponent>();
		glm::mat4 localToGlobaltransform;
		glm::vec3 pos;
		if (transformComponent) {
			localToGlobaltransform = transformComponent->getMatrix();
			pos = transformComponent->getPosition();
		}
		else {
			localToGlobaltransform = glm::mat4(1.0f); //Identity
			pos = glm::vec3(0.f); //Origin
		}
		if (lightingComponent) {
			std::shared_ptr<PointLight> pointlight = lightingComponent->getPointLight();
			std::shared_ptr<SpotLight> spotlight = lightingComponent->getSpotLight();
			std::shared_ptr<DirectionalLight> directionallight = lightingComponent->getDirectionalLight();
			std::shared_ptr<AmbientLight> ambientlight = lightingComponent->getAmbient();
			if (pointlight) renderPipeline->addPointLight(pointlight, localToGlobaltransform);
			if (spotlight) renderPipeline->addSpotLight(spotlight, localToGlobaltransform);
			if (directionallight) renderPipeline->setDirectionalLight(directionallight);
			if (ambientlight) renderPipeline->setAmbientLight(ambientlight);
		}
		if (cameraComponent) {
			CameraPurpose purpose = cameraComponent->getPurpose();
			if (purpose == CameraPurpose::render) {
				cameraComponent->windowSizeChanged(static_cast<float>(window->getWidth()), static_cast<float>(window->getHeight()));
				renderPipeline->setCamera(
					pos,
					localToGlobaltransform * cameraComponent->getViewMatrix(),
					cameraComponent->getProjectionMatrix()
				);
			}
			else if (purpose == CameraPurpose::shadowMap) {
				renderPipeline->setDirectionalLightShadowMapProperties(
					localToGlobaltransform * cameraComponent->getViewMatrix(),
					cameraComponent->getProjectionMatrix(),
					4096,
					4096
				);
			}
		}
		if (rendererComponent) {
			if (rendererComponent->toBeRendered() && modelComponent) {
				std::shared_ptr<Model> model = modelComponent->getModel();
				if (model) renderPipeline->attachRender(model, localToGlobaltransform);
			}
		}
	}
	//Render the output
	renderPipeline->executeRender();
	renderPipeline->flushLists();

	glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui

	gui->draw();
	window->swapBuffers();
}

RenderSystem::~RenderSystem() {
    
}

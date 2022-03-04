#include "RenderSystem.h"


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
	/*  Render Pass   */
	//Setting up window
	renderPipeline->setWindowDimentions(window->getWidth(), window->getHeight());

	//Attach all entity objects to render to render
	for (Entity::Iterator e = scene->begin(); e != scene->end(); e++) {//Willl change, need to do tree traversal
		
		std::shared_ptr<LightingComponent> lightingComponent = e->getComponent<LightingComponent>();
		std::shared_ptr<TransformComponent> transformComponent = e->getComponent<TransformComponent>();
		std::shared_ptr<CameraComponent> cameraComponent = e->getComponent<CameraComponent>();
		std::shared_ptr<ModelComponent> modelComponent = e->getComponent<ModelComponent>();
		std::shared_ptr<RendererComponent> rendererComponent = e->getComponent<RendererComponent>();
		glm::mat4 localToGlobaltransform = glm::mat4(1.0f); //Identity
		glm::vec3 pos = glm::vec3(0.f); //Origin
		if (transformComponent) {
			localToGlobaltransform = transformComponent->getGlobalMatrix();
			pos = glm::vec3(localToGlobaltransform * glm::vec4(0.f, 0.f, 0.f, 1.f));
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
					cameraComponent->getViewMatrix(localToGlobaltransform),
					cameraComponent->getProjectionMatrix()
				);
			}
			else if (purpose == CameraPurpose::shadowMap) {
				renderPipeline->setDirectionalLightShadowMapProperties(
					glm::normalize(glm::vec3(localToGlobaltransform * glm::vec4(0.f,0.f,-1.f,0.f))),
					cameraComponent->getViewMatrix(localToGlobaltransform),
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
	//Flush the render queue
	renderPipeline->flushLists();
	//Draw the GUI ontop
	gui->draw();
	//Swap the drawbuffer
	window->swapBuffers();
}

RenderSystem::~RenderSystem() {
    //Nothing to do here
}

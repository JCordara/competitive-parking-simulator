#include "RenderSystem.h"


RenderSystem::RenderSystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<GuiScene> gui,
    std::shared_ptr<Window> window)
    : scene(scene)
	, guiScene(gui)
    , window(window)
{
    /* Rendering system initialization */
	renderPipeline = std::make_shared<GameRenderPipeline>();
}

void RenderSystem::update() {
	/*  Render Pass   */
	//Setting up window
	renderPipeline->setWindowDimentions(window->getWidth(), window->getHeight());

	//All rendering objects
	for (auto& wp_rc : scene->iterate<RendererComponent>()) {
		auto rc = wp_rc.lock(); if (!rc) continue;
		auto e = rc->getEntity();
		auto mc = e->getComponent<ModelComponent>();
		glm::mat4 localToGlobaltransform = getLocalToGlobalTransformation(e);
		if (rc->toBeRendered() && mc) {
			std::shared_ptr<Model> model = mc->getModel();
			if (model) renderPipeline->attachRender(model, localToGlobaltransform, rc->isTransparent());
		}
	}

	//All Lighting objects
	for (auto& wp_lc : scene->iterate<LightingComponent>()) {
		auto lc = wp_lc.lock(); if (!lc) continue;
		auto e = lc->getEntity();
		glm::mat4 localToGlobaltransform = getLocalToGlobalTransformation(e);
		std::shared_ptr<PointLight> pointlight = lc->getPointLight();
		std::shared_ptr<SpotLight> spotlight = lc->getSpotLight();
		std::shared_ptr<DirectionalLight> directionallight = lc->getDirectionalLight();
		std::shared_ptr<AmbientLight> ambientlight = lc->getAmbient();
		if (pointlight) renderPipeline->addPointLight(pointlight, localToGlobaltransform);
		if (spotlight) renderPipeline->addSpotLight(spotlight, localToGlobaltransform);
		if (directionallight) renderPipeline->setDirectionalLight(directionallight);
		if (ambientlight) renderPipeline->setAmbientLight(ambientlight);
	}

	//All Camera Objects
	for (auto& wp_cc : scene->iterate<CameraComponent>()) {
		auto cc = wp_cc.lock(); if (!cc) continue;
		auto e = cc->getEntity();
		glm::mat4 localToGlobaltransform = getLocalToGlobalTransformation(e);
		glm::vec3 pos = glm::vec3(localToGlobaltransform * glm::vec4(0.f, 0.f, 0.f, 1.f));
		CameraPurpose purpose = cc->getPurpose();
		if ( (playing && purpose == CameraPurpose::render ) || ((!playing) && purpose == CameraPurpose::menu) ) {//render in game mode
			cc->windowSizeChanged(window->getWidth(), window->getHeight());
			renderPipeline->setCamera(
				pos,
				cc->getViewMatrix(localToGlobaltransform),
				cc->getProjectionMatrix(),
				cc->getNearClipPlane(),
				cc->getFarClipPlane()
			);
		}
		else if ((playing && purpose == CameraPurpose::shadowMap) || ((!playing) && purpose == CameraPurpose::shadowMapMenu)) {
			renderPipeline->setDirectionalLightShadowMapProperties(
				glm::normalize(glm::vec3(localToGlobaltransform * glm::vec4(0.f, 0.f, -1.f, 0.f))),
				cc->getViewMatrix(localToGlobaltransform),
				cc->getProjectionMatrix(),
				4096,
				3072
			);
		}
	}

	//Not sure where to do this
	//night fog 0.176f,0.098f,0.384f
	renderPipeline->setTranslucentIntensity(glm::vec3(0.3f, 0.3f, 0.3f));

	//Render the output
	renderPipeline->executeRender();
	//Flush the render queue
	renderPipeline->flushLists();
	//Draw the GUI ontop
	guiScene->draw();
	//Swap the drawbuffer
	window->swapBuffers();
}

void RenderSystem::changeScene(shared_ptr<Scene> newScene) {
	scene = newScene;
}

void RenderSystem::changeGui(shared_ptr<GuiScene> newGui) {
	guiScene = newGui;
}


RenderSystem::~RenderSystem() {
    //Nothing to do here
}

glm::vec3 VectorPlaneProjection(glm::vec3 vec, glm::vec3 normal) {
	return vec - VectorVectorProjection(vec, normal);
}

glm::vec3 VectorVectorProjection(glm::vec3 vec, glm::vec3 vec0) {
	vec0 = normalize(vec0);
	return dot(vec, vec0) * vec0;
}

glm::mat4 getLocalToGlobalTransformation(weak_ptr<Entity> wp_e) {
	auto e = wp_e.lock();// Try to get shared pointer to entity
	if (!e) return glm::mat4(1.0f);
	std::shared_ptr<TransformComponent> transformComponent = e->getComponent<TransformComponent>();
	if (!transformComponent) return glm::mat4(1.0f);
	std::shared_ptr<DescriptionComponent> descriptionComponent = e->getComponent<DescriptionComponent>();
	auto parent = e->parent().lock();
	if(!parent) return transformComponent->getLocalMatrix();
	if(!parent->hasComponent<TransformComponent>()) return transformComponent->getLocalMatrix();
	bool ignoreParentRotations = false;
	bool projectionOfParentOntoYPlane = false;
	if (descriptionComponent) {
		ignoreParentRotations = descriptionComponent->getInteger("Ignore parent rotations in render").has_value();//Dont care about value, use a flag
		projectionOfParentOntoYPlane = descriptionComponent->getInteger("Parent Global Y-Plane Forward Direction Projection").has_value();
	}
	if (ignoreParentRotations) {
		glm::vec3 temp = glm::vec3(parent->getComponent<TransformComponent>()->getGlobalMatrix()[3]);
		return glm::translate(glm::mat4(1.f), temp) * transformComponent->getLocalMatrix();
	}
	else if (projectionOfParentOntoYPlane) {
		// Get the default forward direction
		glm::vec3 Forward = glm::vec3(1.f, 0.f, 0.f);
		if (parent->hasComponent<DescriptionComponent>()) {
			auto f = parent->getComponent<DescriptionComponent>()->getVec3("Forward");
			if (f.has_value()) Forward = f.value();
		}
		//Get new forward direction
		glm::vec3 nForward = normalize(glm::vec3(parent->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(Forward, 0.f)));
		//Get the error correction vector mag
		float magnatude_a = abs(dot(nForward, glm::vec3(0.f, 1.f, 0.f)));
		magnatude_a = (magnatude_a < 0.258f) ? 0.f : magnatude_a - 0.258f;
		//Get the projection onto the x-z plane
		Forward = normalize(VectorPlaneProjection(Forward, glm::vec3(0.f, 1.f, 0.f)));
		nForward = normalize(VectorPlaneProjection(nForward, glm::vec3(0.f, 1.f, 0.f)));
		//Get the error correction vector
		glm::vec3 a = magnatude_a * normalize(cross(nForward, glm::vec3(0.f, 1.f, 0.f)));
		//Find hte angle between
		nForward = normalize(a + nForward);
		float angle = acosf(dot(Forward, nForward));
		glm::vec3 normal = cross(Forward, nForward);
		glm::mat4 rot;
		if (length(normal) < 0.0001f) {
			if (angle > 0.0001f) rot = convert<glm::mat4>(physx::PxQuat(glm::radians(180.f), physx::PxVec3(0.f, 1.f, 0.f)));
			else rot = glm::mat4(1.f);
		}
		else rot = convert<glm::mat4>(physx::PxQuat(angle, convert<PxVec3>(normalize(normal))));
		glm::vec3 temp = glm::vec3(parent->getComponent<TransformComponent>()->getGlobalMatrix()[3]);
		return glm::translate(glm::mat4(1.f), temp) * rot * transformComponent->getLocalMatrix();
	}
	return transformComponent->getGlobalMatrix();
}



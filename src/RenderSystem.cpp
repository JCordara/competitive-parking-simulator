#include "RenderSystem.h"


RenderSystem::RenderSystem(
    std::shared_ptr<Scene> scene,
    std::shared_ptr<Window> window)
    : scene(scene)
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

	//All rendering objects
	for (auto& rc : scene->iterate<RendererComponent>()) {
		auto e = rc->getEntity();
		auto mc = e->getComponent<ModelComponent>();
		glm::mat4 localToGlobaltransform = getLocalToGlobalTransformation(e);
		if (rc->toBeRendered() && mc) {
			std::shared_ptr<Model> model = mc->getModel();
			if (model) renderPipeline->attachRender(model, localToGlobaltransform);
		}
	}

	//All Lighting objects
	for (auto& lc : scene->iterate<LightingComponent>()) {
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
	for (auto& cc : scene->iterate<CameraComponent>()) {
		auto e = cc->getEntity();
		glm::mat4 localToGlobaltransform = getLocalToGlobalTransformation(e);
		glm::vec3 pos = glm::vec3(localToGlobaltransform * glm::vec4(0.f, 0.f, 0.f, 1.f));
		CameraPurpose purpose = cc->getPurpose();
		if (purpose == CameraPurpose::render) {
			if (e->parent()) {
				if (e->parent()->hasComponent<VehicleComponent>()) {
					auto v = e->parent()->getComponent<VehicleComponent>()->vehicle->getRigidDynamicActor()->getLinearVelocity().magnitude();
					v = v - 10.f;
					v = (v < 0.f) ? 0.f : v;
					cc->setFov(glm::radians(100.f + 1.2f*v));//Hacky lol
				}
			}
			cc->windowSizeChanged(window->getWidth(), window->getHeight());
			renderPipeline->setCamera(
				pos,
				cc->getViewMatrix(localToGlobaltransform),
				cc->getProjectionMatrix(),
				cc->getNearClipPlane(),
				cc->getFarClipPlane()
			);
		}
		else if (purpose == CameraPurpose::shadowMap) {
			renderPipeline->setDirectionalLightShadowMapProperties(
				glm::normalize(glm::vec3(localToGlobaltransform * glm::vec4(0.f, 0.f, -1.f, 0.f))),
				cc->getViewMatrix(localToGlobaltransform),
				cc->getProjectionMatrix(),
				8192,//4096,
				4096
			);
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

glm::vec3 VectorPlaneProjection(glm::vec3 vec, glm::vec3 normal) {
	return vec - VectorVectorProjection(vec, normal);
}

glm::vec3 VectorVectorProjection(glm::vec3 vec, glm::vec3 vec0) {
	vec0 = normalize(vec0);
	return dot(vec, vec0) * vec0;
}

glm::mat4 getLocalToGlobalTransformation(sp<Entity> e) {
	std::shared_ptr<TransformComponent> transformComponent = e->getComponent<TransformComponent>();
	if (!transformComponent) return glm::mat4(1.0f);
	std::shared_ptr<DescriptionComponent> descriptionComponent = e->getComponent<DescriptionComponent>();
	if(!e->parent()) transformComponent->getLocalMatrix();
	if (!e->parent()->hasComponent<TransformComponent>()) return transformComponent->getLocalMatrix();
	bool ignoreParentRotations = false;
	bool projectionOfParentOntoYPlane = false;
	if (descriptionComponent) {
		ignoreParentRotations = descriptionComponent->getInteger("Ignore parent rotations in render").has_value();//Dont care about value, use a flag
		projectionOfParentOntoYPlane = descriptionComponent->getInteger("Parent Global Y-Plane Forward Direction Projection").has_value();
	}
	if (ignoreParentRotations) {
		glm::vec3 temp = glm::vec3(e->parent()->getComponent<TransformComponent>()->getGlobalMatrix()[3]);
		return glm::translate(glm::mat4(1.f), temp) * transformComponent->getLocalMatrix();
	}
	else if (projectionOfParentOntoYPlane) {
		// Get the default forward direction
		glm::vec3 Forward = glm::vec3(1.f, 0.f, 0.f);
		if (e->parent()->hasComponent<DescriptionComponent>()) {
			auto f = e->parent()->getComponent<DescriptionComponent>()->getVec3("Forward");
			if (f.has_value()) Forward = f.value();
		}
		//Get new forward direction
		glm::vec3 nForward = normalize(glm::vec3(e->parent()->getComponent<TransformComponent>()->getGlobalMatrix() * glm::vec4(Forward, 0.f)));
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
		glm::vec3 temp = glm::vec3(e->parent()->getComponent<TransformComponent>()->getGlobalMatrix()[3]);
		return glm::translate(glm::mat4(1.f), temp) * rot * transformComponent->getLocalMatrix();
	}
	return transformComponent->getGlobalMatrix();
}



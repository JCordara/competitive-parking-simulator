#include "Application.h"
#include <Random.h>

Application::Application(appSettings& settings): 
	settings(settings)
{
	/* --- App initialization --- */
	glfwInit();
	Time::init();
	Random::init();
	/* --- Application Framework Events--- */
	Events::ExitApplication.registerHandler<Application, &Application::exitApplication>(this);
	/* --- GUI Swap Events --- */
	Events::MainMenu.registerHandler<Application, &Application::setupMainMenu>(this);
	Events::GameGUI.registerHandler<Application, &Application::setupBaseLevelGUI>(this);
	Events::RoundEndGUI.registerHandler<Application, &Application::roundWonMenu>(this);
	Events::GameEndGUI.registerHandler<Application, &Application::gameEndGui>(this);
	Events::GameOptions.registerHandler<Application, &Application::setupOptions>(this);
	/* --- Entity Manipulation Events --- */
	Events::AddPropCar.registerHandler<Application, &Application::addPropCar>(this);
	Events::AddParkingSpace.registerHandler<Application, &Application::addOpenParkingEntity>(this);
	Events::AddAICar.registerHandler<Application, &Application::addAICarEvent>(this);
	/* Framework - used by systems*/
	window = std::make_shared<Window>(80, 80, "Competitive Parking Simulator");
	window->setFullScreen(1);//Set it to primary screen
	//Render loading screen
	auto loadingTexture = std::make_shared<Texture>();
	loadingTexture->load("textures/LoadingScreen.png", GL_LINEAR);
	ImageRender loadingScreenImageRender;
	loadingScreenImageRender.use();
	loadingScreenImageRender.draw(loadingTexture);
	window->swapBuffers();
	//Needs to be after the window constructor (LEAVE HERE)
	Events::Fullscreen.registerHandler<Window, &Window::setFullScreen>(window.get());
	/// <param name="settings"></param>
	scene = std::make_shared<Scene>();
	/* Game systems - update() every frame */
	input = std::make_shared<InputSystem>(window);
	gameplay = std::make_shared<GameplaySystem>(scene);
	physics = std::make_shared<PhysicsSystem>(scene);
	render = std::make_shared<RenderSystem>(scene, guiScene, window);
	audio = std::make_shared<AudioSystem>(scene);

	/* Proceed */
	setupMainMenu();
	playgame = false;
	// --- Loading the models our game uses --- //
	loadModels();
	gameplay->defineMap("modelTransformations/aiNodeLocation.txt", getInstancedTransformationsOrThrow("parkingSpots"), getInstancedTransformationsOrThrow("emptyParkingSpots"));
	/* --- Set up Constaint entities --- */
	generateStaticMap();
	createPlayerEntity(instancedTransformation());

	
}

int Application::play() {
	//Game loop
	while (!window->shouldClose()) {
		// Process input
		input->processInput();
		// Update time-related values
		Time::update();
		//Inform systems
		render->setPlaying(playgame);
		// Fixed time step game loop
		while (Time::takeNextStep()) {
				physics->update();	// Physics update
				audio->update();	// Audio update
		}
		gameplay->update();	// Gameplay / AI update
		render->update(); // Render the current scene
	}
	return 0;
}

// --- entity generation and manipulation ---
void Application::generateStaticMap() {
	// --- Map road ---
	auto mapRoad = scene->addEntity().lock();
	mapRoad->addComponent<ModelComponent>()->setModel(getModelOrThrow("cpsMap_PLotPlane.obj"));
	mapRoad->addComponent<RendererComponent>()->enableRender();
	// --- Map Background Grass ---
	auto mapGrass = scene->addEntity().lock();
	mapGrass->addComponent<ModelComponent>()->setModel(getModelOrThrow("cpsMap_BGGrass.obj"));
	mapGrass->addComponent<RendererComponent>()->enableRender();
	// --- Map Mall ---
	auto mapMall = scene->addEntity().lock();
	mapMall->getComponent<TransformComponent>()->localTranslate(0.0f, -1.0f, 0.0f);
	mapMall->addComponent<ModelComponent>()->setModel(getModelOrThrow("cpsMap_Mall.obj"));
	mapMall->addComponent<RendererComponent>()->enableRender();
	mapMall->addComponent<RigidbodyComponent>()->addActorStaticMesh(
		*getModelOrThrow("cpsMap_Mall.obj"),
		convert<physx::PxTransform>(mapMall->getComponent<TransformComponent>()->getGlobalMatrix()),
		0.5f, 0.5f
	);
	// --- Map Mall Text ---
	auto mapMallText = scene->addEntity().lock();
	mapMallText->getComponent<TransformComponent>()->localTranslate(0.0f, -5.5f, 0.0f);
	mapMallText->addComponent<ModelComponent>()->setModel(getModelOrThrow("cpsMap_MallText.obj"));
	mapMallText->addComponent<RendererComponent>()->enableRender();
	// --- Map Boundary Walls ---
	// - Left Wall -
	auto mapWall1 = scene->addEntity().lock();
	mapWall1->getComponent<TransformComponent>()->localTranslate(0.0f, -1.0f, 0.0f);
	mapWall1->addComponent<ModelComponent>()->setModel(getModelOrThrow("cpsMap_BWall1.obj"));
	mapWall1->addComponent<RigidbodyComponent>()->addActorStaticMesh(
		*getModelOrThrow("cpsMap_BWall1.obj"),
		convert<physx::PxTransform>(mapWall1->getComponent<TransformComponent>()->getGlobalMatrix()),
		0.5f, 0.5f
	);
	// - Right Wall -
	auto mapWall2 = scene->addEntity().lock();
	mapWall2->getComponent<TransformComponent>()->localTranslate(0.0f, -1.0f, 0.0f);
	mapWall2->addComponent<ModelComponent>()->setModel(getModelOrThrow("cpsMap_BWall2.obj"));
	mapWall2->addComponent<RigidbodyComponent>()->addActorStaticMesh(
		*getModelOrThrow("cpsMap_BWall2.obj"),
		convert<physx::PxTransform>(mapWall2->getComponent<TransformComponent>()->getGlobalMatrix()),
		0.5f, 0.5f
	);
	// - Back Wall -
	auto mapWall3 = scene->addEntity().lock();
	mapWall3->getComponent<TransformComponent>()->localTranslate(0.0f, -1.0f, 0.0f);
	mapWall3->addComponent<ModelComponent>()->setModel(getModelOrThrow("cpsMap_BWall3.obj"));
	mapWall3->addComponent<RigidbodyComponent>()->addActorStaticMesh(
		*getModelOrThrow("cpsMap_BWall3.obj"),
		convert<physx::PxTransform>(mapWall3->getComponent<TransformComponent>()->getGlobalMatrix()),
		0.5f, 0.5f
	);
	// --- Render Only Entities ---
	auto InstancedStatic = [ptr = this](
		string modelName,
		string transformationsName,
		int StaticPhysxObjectcode,
		std::optional<physx::PxVec3> halfLengths
	) {
		auto transformations = ptr->getInstancedTransformationsOrThrow(transformationsName);
		auto model = ptr->getModelOrThrow(modelName);
		for (auto i = transformations.begin(); i != transformations.end(); i++) {
			auto entity = ptr->scene->addEntity().lock();
			entity->getComponent<TransformComponent>()->localTranslate(i->location);
			entity->getComponent<TransformComponent>()->localRotate(glm::radians(i->rotationAxisAngle.y), glm::vec3(0.f, 1.f, 0.f));
			entity->addComponent<ModelComponent>()->setModel(model);
			entity->addComponent<RendererComponent>()->enableRender();
			if(StaticPhysxObjectcode == 1)
				entity->addComponent<RigidbodyComponent>()->addActorStaticMesh(
					*model, convert<physx::PxTransform>(entity->getComponent<TransformComponent>()->getGlobalMatrix()),
					0.5f, 0.0005f
				);
			else if (StaticPhysxObjectcode == 2 && halfLengths.has_value())
				entity->addComponent<RigidbodyComponent>()->addActorStaticBox(
					halfLengths.value(),
					convert<physx::PxTransform>(entity->getComponent<TransformComponent>()->getGlobalMatrix())
				);
		}
	};
	auto fail = std::optional<physx::PxVec3>();
	// --- Map Metal Fence ---
	InstancedStatic("cpsMap_MetalFence.obj", "cpsMap_MetalFence.obj", 0, fail);
	// --- Map Warning Wall ---
	InstancedStatic("cpsMap_WarningWall.obj", "cpsMap_WarningWall.obj", 0, fail);
	// --- Map Background Road ---
	InstancedStatic("cpsMap_BGRoad.obj", "cpsMap_BGRoad.obj", 0, fail);
	// --- Map Parking Lines ---
	InstancedStatic("cpsMap_ParkingLine.obj", "cpsMap_ParkingLine.obj", 0, fail);
	// --- Tree Leaves ---
	InstancedStatic("cpsMap_TreeLeaves.obj", "cpsMap_TreeStump.obj & cpsMap_TreeLeaves.obj", 0, fail);
	// --- Mall Lamps ---
	InstancedStatic("cpsMap_MallLamp.obj", "cpsMap_MallLamp.obj", 0, fail);
	// --- Static Mesh Entities ---
	// --- Map Curb ---
	InstancedStatic("cpsMap_Curb.obj", "cpsMap_Curb.obj", 1, fail);
	// --- Map Hedges ---
	InstancedStatic("cpsMap_Hedge.obj", "cpsMap_Hedge.obj", 2, physx::PxVec3(7.0f, 1.43f, 0.8f));
	// --- Map Wood Fence ---
	InstancedStatic("cpsMap_Woodfence.obj", "cpsMap_Woodfence.obj", 1, fail);
	// --- Map Wood Fence Pole ---
	InstancedStatic("cpsMap_WoodfencePole.obj", "cpsMap_WoodfencePole.obj", 1, fail);
	// --- Map Trees ---
	InstancedStatic("cpsMap_TreeStump.obj", "cpsMap_TreeStump.obj & cpsMap_TreeLeaves.obj", 1, fail);
	// --- Map Ramps ---
	InstancedStatic("cpsMap_Ramp.obj", "cpsMap_Ramp.obj", 1, fail);
	// --- Map SpeedBumps ---
	//InstancedStatic("cpsMap_Speedbump.obj", "cpsMap_Speedbump.obj", 1, fail);
	// --- Map Streetlight ---
	InstancedStatic("cpsMap_Streetlight.obj", "cpsMap_Streetlight.obj", 1, fail);
	// --- Enviromental light ---
	auto environmentalLight = scene->addEntity().lock();
	environmentalLight->addComponent<LightingComponent>()->setAmbient(glm::vec3(0.05f, 0.05f, 0.05f));
	environmentalLight->getComponent<LightingComponent>()->setDirectionalLight(glm::vec3(0.3f, 0.3f, 0.3f));
	environmentalLight->addComponent<DescriptionComponent>()->setString("Name", "World Lighting");
	// --- Static Menu Camera ---
	auto menuCamera = scene->addEntity().lock();
	menuCamera->getComponent<TransformComponent>()->setLocalPosition(-30.0f, 15.0f, 20.0f);
	menuCamera->getComponent<TransformComponent>()->setLocalRotation(glm::radians(-45.0f), glm::vec3(1.f, 0.f, 0.f));
	menuCamera->getComponent<TransformComponent>()->localRotate(glm::radians(-45.0f), glm::vec3(0.f, 1.f, 0.f));
	menuCamera->addComponent<CameraComponent>()->setPurpose(CameraPurpose::menu);
	menuCamera->getComponent<CameraComponent>()->setPerspectiveCamera(glm::radians(110.f), 1.f /*Will be modified to the window*/, 5.f, 300.f);
	// --- Static Menu Shadow Camera --
	auto shadowCamera = scene->addEntity().lock();
	shadowCamera->getComponent<TransformComponent>()->setLocalPosition(50.f, 100.0f, 100.0f);
	shadowCamera->getComponent<TransformComponent>()->setLocalRotation(glm::radians(-45.f), physx::PxVec3(1.f, 0.f, 0.f));
	shadowCamera->addComponent<CameraComponent>()->setPurpose(CameraPurpose::shadowMapMenu);
	shadowCamera->getComponent<CameraComponent>()->setOrthographicCamera(300.f, 250.f, 10.f, 300.f);
}

void Application::createCar(string chassisModelName, std::shared_ptr<Entity> ent) {
	// Add wheel entities
	for (int i = 0; i < 4; i++) {
		auto wheel = ent->addChild().lock();
		wheel->addComponent<ModelComponent>()->setModel(getModelOrThrow("car1wheel.obj"));
		wheel->addComponent<RendererComponent>()->enableRender();
		wheel->addComponent<DescriptionComponent>()->setInteger("wheel", 1);
	}
	ent->addComponent<ModelComponent>()->setModel(getModelOrThrow(chassisModelName));
	ent->addComponent<RendererComponent>()->enableRender();
	//Add the two head lights to the player car
	auto lightEntity = ent->addChild().lock();
	lightEntity->addComponent<LightingComponent>()->setSpotLight(glm::vec3(0.94f, 0.89f, 0.54f), glm::vec3(1.f, 0.007f, 0.0002f), glm::radians(20.f), glm::radians(30.f));
	lightEntity->getComponent<TransformComponent>()->setLocalPosition(-0.8f, 0.15f, 1.5f);
	lightEntity->getComponent<TransformComponent>()->setLocalRotation(glm::radians(10.f), glm::vec3(1.f, 0.f, 0.f));

	lightEntity = ent->addChild().lock();
	lightEntity->addComponent<LightingComponent>()->setSpotLight(glm::vec3(0.94f, 0.89f, 0.54f), glm::vec3(1.f, 0.007f, 0.0002f), glm::radians(20.f), glm::radians(30.f));
	lightEntity->getComponent<TransformComponent>()->setLocalPosition(0.8f, 0.15f, 1.5);
	lightEntity->getComponent<TransformComponent>()->setLocalRotation(glm::radians(10.f), glm::vec3(1.f, 0.f, 0.f));
	//-------------------
	ent->addComponent<VehicleComponent>();
	ent->addComponent<DescriptionComponent>()->setVec3("Forward", glm::vec3(0.f, 0.f, 1.f));
	ent->addComponent<DescriptionComponent>()->setVec3("Up", glm::vec3(0.f, 1.f, 0.f));
}

std::shared_ptr<Entity> Application::createPlayerEntity(instancedTransformation transformation) {
	auto playerCar = scene->addEntity().lock();
	playerCar->getComponent<TransformComponent>()->setLocalPosition(transformation.location);
	playerCar->getComponent<TransformComponent>()->setLocalRotation(transformation.rotationAxisAngle.y, glm::vec3(0.0f, 1.0f, 0.0f));
	// --- Main camera --- //
	auto mainCamera = playerCar->addChild().lock();
	mainCamera->getComponent<TransformComponent>()->setLocalPosition(0.0f, 12.0f, -2.0f);
	mainCamera->getComponent<TransformComponent>()->setLocalRotation(glm::radians(-60.0f), glm::vec3(1.f, 0.f, 0.f));
	mainCamera->getComponent<TransformComponent>()->localRotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate to face the other way
	mainCamera->addComponent<CameraComponent>()->setPurpose(CameraPurpose::render);
	mainCamera->getComponent<CameraComponent>()->setPerspectiveCamera(glm::radians(100.f), 1.f /*Will be modified to the window*/, 1.f, 130.f);
	mainCamera->addComponent<DescriptionComponent>()->setInteger("Parent Global Y-Plane Forward Direction Projection", 1);
	// --- Shadow map camera --- //
	auto shadowCamera = playerCar->addChild().lock();
	shadowCamera->getComponent<TransformComponent>()->setLocalPosition(0, 100.0f, 100.0f);
	shadowCamera->getComponent<TransformComponent>()->setLocalRotation(glm::radians(-45.f), physx::PxVec3(1.f, 0.f, 0.f));
	shadowCamera->addComponent<CameraComponent>()->setPurpose(CameraPurpose::shadowMap);
	shadowCamera->getComponent<CameraComponent>()->setOrthographicCamera(200.f, 150.f, 10.f, 300.f);
	shadowCamera->addComponent<DescriptionComponent>()->setInteger("Ignore parent rotations in render", 1);
	// --- Vehicle --- //
	createCar("car1chassis.obj", playerCar);
	// --- Controller --- //
	auto playerController = playerCar->addComponent<ControllerComponent>();
	playerController->createAxis(GLFW_KEY_W, GLFW_KEY_S, &Events::VehicleAccelerate);
	playerController->createAxis(GLFW_KEY_UP, GLFW_KEY_DOWN, &Events::VehicleAccelerate);
	playerController->createAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, &Events::VehicleAccelerate, ControlAxis::AXIS);
	playerController->createAxis(GLFW_KEY_D, GLFW_KEY_A, &Events::VehicleSteer);
	playerController->createAxis(GLFW_KEY_RIGHT, GLFW_KEY_LEFT, &Events::VehicleSteer);
	playerController->createAxis(GLFW_GAMEPAD_AXIS_LEFT_X, &Events::VehicleSteer);
	playerController->bindInput(GLFW_KEY_F, &Events::VehicleFlip);
	playerController->bindInput(GLFW_GAMEPAD_BUTTON_CIRCLE, &Events::VehicleFlip);
	playerController->bindInput(GLFW_KEY_LEFT_SHIFT, &Events::VehicleHandbrake);
	playerController->bindInput(GLFW_GAMEPAD_BUTTON_SQUARE, &Events::VehicleHandbrake);
	// --- Indicator for player car --- //
	playerCar->addComponent<DescriptionComponent>()->setString("Name", "Player Car");
	playerCar->getComponent<DescriptionComponent>()->setVec3("Spawn Position", transformation.location);
	playerCar->getComponent<DescriptionComponent>()->setRealNumber("Spawn Y-Rotation", transformation.rotationAxisAngle.y);
	// --- Audio --- //
	playerCar->addComponent<AudioComponent>()->addEngineSound("audio/engine.wav", playerCar->getComponent<VehicleComponent>());
	audio->setListener(mainCamera->getComponent<TransformComponent>());
	return playerCar;
}

std::shared_ptr<Entity> Application::addAICar(string alias) {
	auto aiCar = scene->addEntity().lock();
	createCar("car2chassis.obj", aiCar);
	aiCar->addComponent<AudioComponent>()->addSound(AudioTrigger::Collision, "audio/oof.wav");
	// --- Indicator for Ai car --- //
	aiCar->addComponent<DescriptionComponent>()->setString("Name", alias);
	// --- Ai Component --- //
	aiCar->addComponent<AiComponent>();
	return aiCar;
}

std::shared_ptr<Entity> Application::addDynamicObect(string alias, string modelName, instancedTransformation transformation) {
	auto object = scene->addEntity().lock();
	object->getComponent<TransformComponent>()->localTranslate(transformation.location);
	object->getComponent<TransformComponent>()->localRotate(glm::radians(transformation.rotationAxisAngle.y), glm::vec3(0.f, 1.f, 0.f));
	object->addComponent<ModelComponent>()->setModel(getModelOrThrow(modelName));
	object->addComponent<RigidbodyComponent>()->addActorDynamic(
		*getModelOrThrow(modelName),
		convert<physx::PxTransform>(object->getComponent<TransformComponent>()->getGlobalMatrix())
	);
	object->addComponent<RendererComponent>()->enableRender();
	// --- Indicator for ent --- //
	object->addComponent<DescriptionComponent>()->setString("Name", alias);
	return object;
}

std::shared_ptr<Entity> Application::addTriggerBoxEntity(string alias, string modelName, instancedTransformation transformation, PxBoxGeometry boxgeom) {
	auto entity = scene->addEntity().lock();
	entity->getComponent<TransformComponent>()->localTranslate(transformation.location);
	entity->getComponent<TransformComponent>()->localRotate(glm::radians(transformation.rotationAxisAngle.y), glm::vec3(0.f, 1.f, 0.f));
	entity->addComponent<RendererComponent>()->enableRender();
	entity->getComponent<RendererComponent>()->enableTransparentRendering();
	entity->addComponent<ModelComponent>()->setModel(getModelOrThrow(modelName));
	// --- Indicator for ent --- //
	entity->addComponent<DescriptionComponent>()->setString("Name", alias);
	entity->addComponent<VolumeTriggerComponent>()->createVolumeShape(PxTransform(convert<PxVec3>(transformation.location)), boxgeom);
	entity->addComponent<DescriptionComponent>()->setVec3("Forward", glm::vec3(0.f, 0.f, 1.f));
	entity->addComponent<DescriptionComponent>()->setVec3("Up", glm::vec3(0.f, 1.f, 0.f));
	return entity;
}
/* --- Entity Manipulation Events --- */
void Application::addOpenParkingEntity(string alias, instancedTransformation transformation) {
	addTriggerBoxEntity(alias, "cpsMap_ParkingIndicator.obj", transformation, PxBoxGeometry(1.f, 1.f, 1.f));
}
void Application::addPropCar(string alias, instancedTransformation transformation) {
	string modelName;
	switch (Random::randomInt(0, 3)) {
		case 0:
			modelName = "car3.obj";
			break;
		case 1:
			modelName = "car4.obj";
			break;
		case 2:
			modelName = "sedan.obj";
			break;
		default:
			modelName = "truck.obj";
			break;
	}
	auto car = addDynamicObect(alias, modelName, transformation);
	car->getComponent<DescriptionComponent>()->setVec3("Spawn Position", transformation.location);
	car->getComponent<DescriptionComponent>()->setRealNumber("Spawn Y-Rotation", transformation.rotationAxisAngle.y);
}
void Application::addAICarEvent(string alias) {
	addAICar(alias);
}
// --- Model Loading --------------------------------------------------------------
void Application::loadModels() {
	// --- Car Models ---
	loadModel("car1chassis.obj", glm::vec3(1.0f, 1.0f, 1.0f));
	loadModel("car1wheel.obj", glm::vec3(1.0f, 1.0f, 1.0f));
	loadModel("car2chassis.obj", glm::vec3(.6f, .6f, .6f));
	loadModel("car3.obj", glm::vec3(.8f, .5f, .6f));
	loadModel("car4.obj", glm::vec3(.8f, .5f, .6f));
	loadModel("sedan.obj", glm::vec3(.8f, .5f, .6f));
	loadModel("truck.obj", glm::vec3(.8f, .5f, .6f));

	// --- Static Map Models ---
	loadModel("cpsMap_PLotPlane.obj", glm::vec3(.5f, .1f, .2f));
	loadModel("cpsMap_Mall.obj", glm::vec3(.5f, .1f, .2f));
	loadModel("cpsMap_MallText.obj", glm::vec3(.5f, .7f, .2f));
	loadModel("cpsMap_BWall1.obj", glm::vec3(.5f, .1f, .2f));
	loadModel("cpsMap_BWall2.obj", glm::vec3(.5f, .1f, .2f));
	loadModel("cpsMap_BWall3.obj", glm::vec3(.5f, .1f, .2f));
	loadModel("cpsMap_BGGrass.obj", glm::vec3(.1f, .1f, .1f));

	// --- Instantiated Map Models ---
	loadModel("cpsMap_MetalFence.obj", glm::vec3(.9f, .5f, .1f));
	loadInstancedTransformations("cpsMap_MetalFence.obj", "metalfenceLocation.txt", "metalfenceRotation.txt");
	loadModel("cpsMap_Curb.obj", glm::vec3(.2f, .5f, .5f));
	loadInstancedTransformations("cpsMap_Curb.obj", "curbLocation.txt", "curbRotation.txt");
	loadModel("cpsMap_Hedge.obj", glm::vec3(.6f, .8f, .5f));
	loadInstancedTransformations("cpsMap_Hedge.obj", "hedgeLocation.txt", "hedgeRotation.txt");
	loadModel("cpsMap_Woodfence.obj", glm::vec3(.3, .5, 1.0f));
	loadInstancedTransformations("cpsMap_Woodfence.obj", "woodfenceLocation.txt", "woodfenceRotation.txt");
	loadModel("cpsMap_WoodfencePole.obj", glm::vec3(.3, .5, 1.0f));
	loadInstancedTransformations("cpsMap_WoodfencePole.obj", "woodfencePoleLocation.txt", "woodfencePoleRotation.txt");
	loadModel("cpsMap_ParkingLine.obj", glm::vec3(.5f, .1f, .2f));
	loadInstancedTransformations("cpsMap_ParkingLine.obj", "parkingLinesLocation.txt", "parkingLinesRotation.txt");
	loadModel("cpsMap_TreeStump.obj", glm::vec3(.1f, .4f, .2f));
	loadModel("cpsMap_TreeLeaves.obj", glm::vec3(.1f, .4f, .2f));
	loadInstancedTransformations("cpsMap_TreeStump.obj & cpsMap_TreeLeaves.obj", "treeLocation.txt", "");
	loadModel("cpsMap_WarningWall.obj", glm::vec3(.9f, .5f, .1f));
	loadInstancedTransformations("cpsMap_WarningWall.obj", "warningWallLocation.txt", "warningWallRotation.txt");
	loadModel("cpsMap_BGRoad.obj", glm::vec3(.5f, .1f, .2f));
	loadInstancedTransformations("cpsMap_BGRoad.obj", "roadLocation.txt", "");
	loadModel("cpsMap_ParkingIndicator.obj", glm::vec3(.5f, .1f, .2f));
	loadModel("cpsMap_Ramp.obj", glm::vec3(.6f, .4f, .6f));
	loadInstancedTransformations("cpsMap_Ramp.obj", "rampLocation.txt", "rampRotation.txt");
	loadModel("cpsMap_Speedbump.obj", glm::vec3(.3f, .5f, .9f));
	loadInstancedTransformations("cpsMap_Speedbump.obj", "speedbumpLocation.txt", "speedbumpRotation.txt");
	loadModel("cpsMap_Streetlight.obj", glm::vec3(.1f, .2f, .1f));
	loadInstancedTransformations("cpsMap_Streetlight.obj", "streetlightLocation.txt", "streetlightRotation.txt");
	loadModel("cpsMap_MallLamp.obj", glm::vec3(.8f, .5f, .3f));
	loadInstancedTransformations("cpsMap_MallLamp.obj", "malllampLocation.txt", "malllampRotation.txt");

	// --- Loading Game transformations ---
	loadInstancedTransformations("parkingSpots", "parkingSpotLocation.txt", "parkingSpotRotation.txt");
	loadInstancedTransformations("emptyParkingSpots", "emptySpotSubsetLocation.txt", "emptySpotSubsetRotation.txt");
}

void Application::loadModel(string filename, glm::vec3 col) {
	loadedModels.push_back(std::pair<string, std::shared_ptr<Model>>(string(filename), std::make_shared<Model>("models/" + filename, col)));
}

std::optional<std::shared_ptr<Model>> Application::retrieveModel(string name) {
	for (auto it = loadedModels.begin(); it != loadedModels.end(); it++)
		if (it->first == name) return it->second;
	return std::nullopt;
}

void Application::loadInstancedTransformations(string modelKey, string positionTransformation, string axisAngleRotationTransformation) {
	std::vector<glm::vec3> pos, rot;
	std::vector<float> temp;
	if(!positionTransformation.empty())
		pos = collectGLMVecFromFile("modelTransformations/" + positionTransformation, pos);
	if (!axisAngleRotationTransformation.empty())
		temp = collectfloatFromFile("modelTransformations/" + axisAngleRotationTransformation, temp);
	instancedTransformation tempt;
	std::vector<instancedTransformation> list;
	for (int i = 0; i < pos.size() || i < temp.size(); i++) {
		tempt.location = (i < pos.size()) ? pos[i] : glm::vec3(0.f, 0.f, 0.f);
		tempt.rotationAxisAngle = (i < temp.size()) ? glm::vec3(0.f,temp[i],0.f) : glm::vec3(0.f, 0.f, 0.f);
		list.push_back(tempt);
	}
	loadedInstancedTransformations.push_back(std::pair<string, std::vector<instancedTransformation>>(modelKey, list));
}

std::optional<std::vector<instancedTransformation>> Application::retrieveInstancedTransformations(string name) {
	for (auto it = loadedInstancedTransformations.begin(); it != loadedInstancedTransformations.end(); it++)
		if (it->first == name) return it->second;
	return std::nullopt;
}

std::shared_ptr<Model> Application::getModelOrThrow(string name) {
	auto m = retrieveModel(name);
	if (!m.has_value())
		throw std::exception("No model exists");
	return m.value();
}

std::vector<instancedTransformation> Application::getInstancedTransformationsOrThrow(string name) {
	auto m = retrieveInstancedTransformations(name);
	if (!(m.has_value()))
		throw std::exception("No list exists");
	return m.value();
}

// --- GUI Manager Function ---
void Application::setupMainMenu() {
	std::shared_ptr<Menu> menu = std::make_shared<Menu>(1, 3, 0.1f);
	guiScene = std::make_shared<GuiScene>(window); // Reset gui
	std::vector<string> names = { "Play","Options","Exit" };
	guiScene->addButton(menu->layout[0][0].positionX, menu->layout[0][0].positionY,"Play", Events::NewGame, 1);
	guiScene->addButton(menu->layout[0][1].positionX, menu->layout[0][1].positionY,
		"Options", Events::GameOptions, 1);
	guiScene->addButton(menu->layout[0][2].positionX, menu->layout[0][2].positionY,"Exit", Events::ExitApplication, 1);
	render->changeGui(guiScene);
	playgame = false;
}

void Application::setupOptions() {
	std::shared_ptr<Menu> menu = std::make_shared<Menu>(1, 4, 0.1f);
	guiScene = std::make_shared<GuiScene>(window); // Reset gui
	guiScene->addSlider(menu->layout[0][0].positionX, menu->layout[0][0].positionY, "Starting Number of AI", Events::ChangeNumberOfAI, gameplay->getStartingAi_number(), 1, 8);
	std::vector<std::string> list = { "Windowed" };
	for (int i = 0; i < window->numberOfMonitors(); i++)
		list.push_back(string("Monitor ") + std::to_string(i));
	guiScene->addCombo(menu->layout[0][1].positionX, menu->layout[0][1].positionY, "FullScreen monitor", list, Events::Fullscreen, (window->getCurrentMonitorNumber()));
	guiScene->addSlider(menu->layout[0][2].positionX, menu->layout[0][2].positionY, "Music Volume", Events::ChangeMusicVolume, 0.1f);
	guiScene->addButton(menu->layout[0][3].positionX, menu->layout[0][3].positionY, "Main Menu", Events::EndGame, 1);
	render->changeGui(guiScene);
	playgame = false;
}

void Application::setupBaseLevelGUI() {
	guiScene = std::make_shared<GuiScene>(window); // Reset gui
	guiScene->addButton(0.01f,0.9f, "Quit to Main Menu", Events::EndGame, 1);
	guiScene->addButton(0.01f, 0.95f, "Exit", Events::ExitApplication, 1);
	guiScene->addLabel( 0.05f, 0.01f, string("Contestants Remaining: ")  + std::to_string(gameplay->getCurrentAi_number() + 1), 2);
	guiScene->addDynamicLabel(0.5f, 0.01f, gameplay->getDisplayString(), 2);
	//ImGui::ProgressBar(float fraction, const ImVec2 & size_arg, const char* overlay)
	render->changeGui(guiScene);
	playgame = true;
}
void Application::roundWonMenu() {
	std::shared_ptr<Menu> menu = std::make_shared<Menu>(1, 2, 0.1f);
	guiScene = std::make_shared<GuiScene>(window); // Reset gui
	guiScene->addButton(menu->layout[0][0].positionX, menu->layout[0][0].positionY, "Next Round", Events::NextRound, 1);
	guiScene->addButton(menu->layout[0][1].positionX, menu->layout[0][1].positionY, "Main Menu", Events::EndGame, 1);
	guiScene->addLabel(0.3f, 0.01f, "Spectating Mode", 2);
	render->changeGui(guiScene);
	playgame = false;
}
void Application::gameEndGui(string message) {
	std::shared_ptr<Menu> menu = std::make_shared<Menu>(1, 2, 0.1f);
	guiScene = std::make_shared<GuiScene>(window); // Reset gui
	guiScene->addLabel(menu->layout[0][0].positionX, menu->layout[0][0].positionY, message, 2);
	guiScene->addButton(menu->layout[0][1].positionX, menu->layout[0][1].positionY, "Main Menu", Events::EndGame, 1);
	guiScene->addLabel(0.3f, 0.01f, "Spectating Mode", 2);
	render->changeGui(guiScene);
	playgame = false;
}
// --- Destructor --------------------------------------------------------------
void Application::exitApplication() {
	glfwSetWindowShouldClose(*window, 1);
}
Application::~Application() {
	//Clean-up
	glfwTerminate();
}
// --- Application settings loading ---------------------------------------------
appSettings defaultSettings() {
	appSettings settings;
	settings.version = "0.0.0";
	return settings;
}
appSettings loadSettings(std::string filepath) {
	//TODO: Actually load settings;
	appSettings settings = defaultSettings();
	return settings;
}

vector<glm::vec3> collectGLMVecFromFile(string filepath, vector<glm::vec3> vec) {
	std::ifstream file(filepath);
	std::string str;
	std::string space = " ";
	size_t pos = 0;
	float x, y, z;
	while (std::getline(file, str)) {
		int counter = 0;
		while ((pos = str.find(space)) != string::npos) {
			counter++;
			if (counter == 1) x = std::stof(str.substr(0, pos));
			else if (counter == 2) y = std::stof(str.substr(0, pos));
			str.erase(0, pos + space.length());
		}
		z = std::stof(str);
		vec.push_back(glm::vec3(x, y, z));
	}
	file.close();
	return vec;
}

vector<float> collectfloatFromFile(string filepath, vector<float> vec) {
	std::ifstream file(filepath);
	std::string str;
	while (std::getline(file, str))vec.push_back(std::stof(str));
	file.close();
	return vec;
}



#include "Application.h"
#include <Random.h>

#define SPAWN_PROP_CARS 1
const unsigned int g_numAiCars = 1;

unsigned int playerId = 0;
std::vector<unsigned int> aiList;
std::unordered_map<unsigned int, int> scores;
std::shared_ptr<Entity> playerCar;

//Model Vectors
std::vector<glm::vec3> hedgeLocation;
std::vector<float> hedgeRotation;
std::vector<glm::vec3> curbLocation;
std::vector<float> curbRotation;
std::vector<glm::vec3> metalFenceLocation;
std::vector<float> metalFenceRotation;
std::vector<glm::vec3> warningWallLocation;
std::vector<float> warningWallRotation;
std::vector<glm::vec3> woodFenceLocation;
std::vector<float> woodFenceRotation;
std::vector<glm::vec3> woodFencePoleLocation;
std::vector<float> woodFencePoleRotation;
std::vector<glm::vec3> treeLocation;
std::vector<glm::vec3> parkingLineLocation;
std::vector<float> parkingLineRotation;
std::vector<glm::vec3> roadLocation;
std::vector<glm::vec3> rampLocation;
std::vector<float> rampRotation;

//Parking Spot Vectors
std::vector<glm::vec3> parkingSpotLocation;
std::vector<float> parkingSpotRotation;
std::vector<glm::vec3> emptySubsetLocation;
std::vector<float> emptySubsetRotation;
//USE THESE VECTORS FOR EMPTY PARKING SPOTS
std::vector<glm::vec3> emptyParkingSpotLocation;
std::vector<float> emptyParkingSpotRotation;

//AI Node Vectors
std::vector<glm::vec3> aiNodeLocation;
std::vector<std::string> aiNodeType;
std::vector<int> aiNodeArea;

void collectAINodeVectors(){
	std::ifstream file("../../res/modelTransformations/aiNodeLocation.txt");
	std::string str;
	std::string space = " ";
	std::string unsc = "_";
	size_t pos = 0;

	float x, y, z;

	while(std::getline(file, str)) {
		int counter = 0;

		if((pos = str.find(unsc)) != string::npos){
			aiNodeType.push_back(str.substr(0, pos));
			str.erase(0, pos + unsc.length());
		}
		
		while((pos = str.find(space)) != string::npos) {
			counter++;
			if(counter == 1){
				aiNodeArea.push_back(std::stoi(str.substr(0, pos)));
			} else if(counter == 2){
				x = std::stof(str.substr(0, pos));
			} else if(counter == 3){
				y = std::stof(str.substr(0, pos));
			}

			str.erase(0, pos + space.length());
		}

		z = std::stof(str);

		aiNodeLocation.push_back(glm::vec3(x,y,z));
	}

	file.close();
}

vector<glm::vec3> collectGLMVecFromFile(string filepath, vector<glm::vec3> vec){
	std::ifstream file(filepath);
	std::string str;
	std::string space = " ";
	size_t pos = 0;

	float x, y, z;

	while(std::getline(file, str)) {
		int counter = 0;
		while((pos = str.find(space)) != string::npos) {
			counter++;
			if(counter == 1){
				x = std::stof(str.substr(0, pos));
			} else if(counter == 2){
				y = std::stof(str.substr(0, pos));
			}

			str.erase(0, pos + space.length());
		}

		z = std::stof(str);

		vec.push_back(glm::vec3(x,y,z));
	}

	file.close();

	return vec;
}

vector<float> collectfloatFromFile(string filepath, vector<float> vec){
	std::ifstream file(filepath);
	std::string str;

	while(std::getline(file, str)) {
		vec.push_back(std::stof(str));
	}

	file.close();

	return vec;
}

void Application::gameStart() {
	for (int i = 0; i < menuStack.size(); i++) {
		menuStack.pop();
	}
	std::cout << "TEST" << std::endl;
	setupBaseLevelGUI();
	Events::GameStart.broadcast();
	render->setPlaying(true);
}

Application::Application(appSettings& settings): 
	settings(settings)
{
	//Register the game start event handler
	Events::GamePlay.registerHandler<Application,
		&Application::gameStart>(this);

	//App initialization
	glfwInit();
	Time::init();
	Random::init();
	/* Framework - used by systems*/
	window   = std::make_shared<Window>(1200, 800, "Competitive Parking Simulator");
	scene    = std::make_shared<Scene>();
	
	//window   = std::make_shared<Window>(1200, 800, "Test Window");
	guiScene = std::make_shared<GuiScene>(window);

	/* Game systems - update() every frame */
	input    = std::make_shared<InputSystem>(window);
	gameplay = std::make_shared<GameplaySystem>(scene);
	physics  = std::make_shared<PhysicsSystem>(scene);
	render   = std::make_shared<RenderSystem>(scene, guiScene, window);
	audio    = std::make_shared<AudioSystem>(scene);

	// Scene and Menu Logic
	setupBaseLevel(scene);// Can change later with appsettings logic
	setupMainMenu();
	sceneQueue.push(scene);

}

int Application::play() {

	// Invoke observers of the GameStart event
	

	//Game loop
	while (!window->shouldClose()) {

		// Process input
		input->processInput();

		// Update time-related values
		Time::update();

		// Fixed time step game loop
		while (Time::takeNextStep()) {
			if (menuStack.size() == 0) {
				if (scores[playerId] >= 20) {//|| scores[aiList[0]] >= 5) {
				}
				else {
					//gameplay->update();	// Gameplay / AI update
					physics->update();	// Physics update
					audio->update();	// Audio update
				}
			}
		}
		gameplay->update();	// Gameplay / AI update
		//audio->update();	// Audio update
		// Render the current scene
		render->update();
	}

	return 0;
}

void Application::setupMainMenu() {
	int columnNum = 1;
	int rowNum = 3;
	float buttonSizeOffset = 0.1;
	std::shared_ptr<Menu> menu = std::make_shared<Menu>(columnNum, rowNum, buttonSizeOffset);
	menuStack.push(menu);
	guiScene = std::make_shared<GuiScene>(window); // Reset gui
	std::vector<string> names = { "Play","Options","Exit" };
	//std::vector<Event<void>> events = { Events::GamePlay, Events::GameOptions, Events::GameExit };
	/*
	for (int i = 0; i < menu->layout.size(); i++) {
		for (int j = 0; j < menu->layout[i].size(); j++) {
			guiScene->addButton(menu->layout[i][j].positionX,
								menu->layout[i][j].positionY,
								names[(i*rowNum)+j],
								events[(i*rowNum)+j],
								1);
		}
	}*/
	guiScene->addButton(menu->layout[0][0].positionX, menu->layout[0][0].positionY,
						"Play", Events::GamePlay, 1);
	guiScene->addButton(menu->layout[0][1].positionX, menu->layout[0][1].positionY,
						"Options", Events::GameOptions, 1);
	guiScene->addButton(menu->layout[0][2].positionX, menu->layout[0][2].positionY,
						"Exit", Events::GameExit, 1);
	render->setPlaying(false);
	render->changeGui(guiScene);
}

void Application::setupBaseLevelGUI() {
	guiScene = std::make_shared<GuiScene>(window); // Reset gui
	guiScene->addSlider(0.01f, 0.1f, "Music Volume", Events::ChangeMusicVolume, 0.1f);
	guiScene->addSlider(0.01f, 0.2f, "AI Opponents", Events::ChangeNumAI, 4, 0, 10);
	render->changeGui(guiScene);
}

void Application::setupBaseLevel(shared_ptr<Scene> scene) {
/* --------------------- Game World Description ------------------------ */
	// Example gui creation
	// guiScene->addLabel(0.5f, 0.0f, "Test label em0", 0);
	// guiScene->addLabel(0.5f, 0.2f, "Test label em1", 1);
	// guiScene->addLabel(0.5f, 0.4f, "Test label em2", 2);
	// guiScene->addLabel(0.05f, 0.6f, "Test label em3", 3);
	// guiScene->addButton(0.1f, 0.7f, "Button em0", Events::CarUnParked, 0);
	// guiScene->addButton(0.4f, 0.7f, "Button em1", Events::GameStart, 1);
	// guiScene->addButton(0.7f, 0.7f, "Button em2", Events::GameStart, 2);
	// guiScene->addCheckbox(0.3f, 0.3f, "Test checkbox", Events::TestUiEvent);
	setupBaseLevelGUI();

	/* --------------------- Game World Description ------------------------ */
	//render->setPlaying(true);
// --- Entities ---
	playerCar = scene->addEntity();
	vector<sp<Entity>> aiCars;

	auto mapRoad = scene->addEntity();
	auto mapMall = scene->addEntity();
	auto mapMallText = scene->addEntity();
	auto mapGrass = scene->addEntity();
	auto mapWall1 = scene->addEntity();
	auto mapWall2 = scene->addEntity();
	auto mapWall3 = scene->addEntity();

	auto triggerBox = scene->addEntity();

	auto environmentalLight = scene->addEntity();
	auto mainCamera = playerCar->addChild();
	auto shadowCamera = playerCar->addChild();
	auto menuCamera = scene->addChild();

	// --- Car Models ---
	auto modelPlayerCar = std::make_shared<Model>(
		"models/car1chassis.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelWheel = std::make_shared<Model>(
		"models/car1wheel.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelAiCar = std::make_shared<Model>(
		"models/car2chassis.obj", glm::vec3(.6f, .6f, .6f));

	auto modelPropCar1 = std::make_shared<Model>(
		"models/car3.obj", glm::vec3(.8f, .5f, .6f));

	auto modelPropCar2 = std::make_shared<Model>(
		"models/car4.obj", glm::vec3(.8f, .5f, .6f));

	auto modelPropCar3 = std::make_shared<Model>(
		"models/sedan.obj", glm::vec3(.8f, .5f, .6f));

	auto modelPropCar4 = std::make_shared<Model>(
		"models/truck.obj", glm::vec3(.8f, .5f, .6f));


	// --- Static Map Models ---
	auto modelMapRoad = std::make_shared<Model>(
		"models/cpsMap_PLotPlane.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapMall = std::make_shared<Model>(
		"models/cpsMap_Mall.obj", glm::vec3(.5f, .1f, .2f));
	
	auto modelMapMallText = std::make_shared<Model>(
		"models/cpsMap_MallText.obj", glm::vec3(.5f, .7f, .2f));

	auto modelMapWall1 = std::make_shared<Model>(
		"models/cpsMap_BWall1.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapWall2 = std::make_shared<Model>(
		"models/cpsMap_BWall2.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapWall3 = std::make_shared<Model>(
		"models/cpsMap_BWall3.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapGrass = std::make_shared<Model>(
		"models/cpsMap_BGGrass.obj", glm::vec3(.1f, .1f, .1f));


	// --- Instantiated Map Models ---
	auto modelMapMetalFence = std::make_shared<Model>(
		"models/cpsMap_MetalFence.obj", glm::vec3(.9f, .5f, .1f));

	auto modelMapCurb = std::make_shared<Model>(
		"models/cpsMap_Curb.obj", glm::vec3(.2f, .5f, .5f));

	auto modelMapHedge = std::make_shared<Model>(
		"models/cpsMap_Hedge.obj", glm::vec3(.6f, .8f, .5f));

	auto modelMapWoodFence = std::make_shared<Model>(
		"models/cpsMap_Woodfence.obj", glm::vec3(.3, .5, 1.0f));

	auto modelMapWoodFencePole = std::make_shared<Model>(
		"models/cpsMap_WoodfencePole.obj", glm::vec3(.3, .5, 1.0f));

	auto modelMapParkingLine = std::make_shared<Model>(
		"models/cpsMap_ParkingLine.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapTreeStump = std::make_shared<Model>(
		"models/cpsMap_TreeStump.obj", glm::vec3(.1f, .4f, .2f));

	auto modelMapTreeLeaves = std::make_shared<Model>(
		"models/cpsMap_TreeLeaves.obj", glm::vec3(.1f, .4f, .2f));

	auto modelMapWarningWall = std::make_shared<Model>(
		"models/cpsMap_WarningWall.obj", glm::vec3(.9f, .5f, .1f));
	
	auto modelMapBGRoad = std::make_shared<Model>(
		"models/cpsMap_BGRoad.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapParkingIndicator= std::make_shared<Model>(
		"models/cpsMap_ParkingIndicator.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapRamp= std::make_shared<Model>(
		"models/cpsMap_Ramp.obj", glm::vec3(.6f, .4f, .6f));


	// --- Directional light ---
	environmentalLight->addComponent<LightingComponent>(); 
	environmentalLight->getComponent<LightingComponent>()->setAmbient(glm::vec3(0.05f, 0.05f, 0.05f));
	environmentalLight->getComponent<LightingComponent>()->setDirectionalLight(glm::vec3(0.3f, 0.3f, 0.3f));

	// --- Main camera ---
	auto mainCameraTransform = mainCamera->getComponent<TransformComponent>();
	mainCameraTransform->setLocalPosition(0.0f, 12.0f, -2.0f);
	mainCameraTransform->setLocalRotation(glm::radians(-60.0f), glm::vec3(1.f, 0.f, 0.f));
	mainCameraTransform->localRotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate to face the other way

	auto mainCamerCam = mainCamera->addComponent<CameraComponent>();
	mainCamerCam->setPerspectiveCamera(glm::radians(100.f), 1.f /*Will be modified to the window*/, 1.f, 130.f);
	mainCamerCam->setPurpose(CameraPurpose::render);

	auto MainDescription = mainCamera->addComponent<DescriptionComponent>();
	MainDescription->setInteger("Parent Global Y-Plane Forward Direction Projection", 1);
	// --- Menu camera ---
	auto menuCameraTransform = menuCamera->getComponent<TransformComponent>();
	menuCameraTransform->setLocalPosition(-30.0f, 15.0f, 20.0f);
	menuCameraTransform->setLocalRotation(glm::radians(-45.0f), glm::vec3(1.f, 0.f, 0.f));
	menuCameraTransform->localRotate(glm::radians(-45.0f), glm::vec3(0.f, 1.f, 0.f));
	//menuCameraTransform->localRotate(glm::radians(.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate to face the other way

	auto menuCamerCam = menuCamera->addComponent<CameraComponent>();
	menuCamerCam->setPerspectiveCamera(glm::radians(110.f), 1.f /*Will be modified to the window*/, 5.f, 300.f);
	menuCamerCam->setPurpose(CameraPurpose::menu);
	// --- Shadow map camera ---
	auto shadowCameraTransform = shadowCamera->getComponent<TransformComponent>();
	auto q1 = physx::PxQuat(glm::radians(-45.f), physx::PxVec3(1.f, 0.f, 0.f));
	//auto q2 = physx::PxQuat(glm::radians(-90.f), physx::PxVec3(0.f, 1.f, 0.f));
	shadowCameraTransform->setLocalPosition(0, 100.0f, 100.0f);
	shadowCameraTransform->setLocalRotation(q1);

	auto shadowCameraCam = shadowCamera->addComponent<CameraComponent>();
	shadowCameraCam->setOrthographicCamera(200.f, 150.f, 10.f, 300.f);
	shadowCameraCam->setPurpose(CameraPurpose::shadowMap);

	auto shadowCameraDesc = shadowCamera->addComponent<DescriptionComponent>();
	shadowCameraDesc->setInteger("Ignore parent rotations in render", 1);


	// --- Player car ---	
	// Add wheel entities
	for (int i = 0; i < 4; i++) {
		auto wheel = playerCar->addChild();

		auto mc = wheel->addComponent<ModelComponent>();
		mc->setModel(modelWheel);

		auto rc = wheel->addComponent<RendererComponent>();
		rc->enableRender();

		auto desc = wheel->addComponent<DescriptionComponent>();
		desc->setInteger("wheel", 1);
	}

	auto playerModel = playerCar->addComponent<ModelComponent>();
	playerModel->setModel(modelPlayerCar);

	auto playerRender = playerCar->addComponent<RendererComponent>();
	playerRender->enableRender();
	//Add the two head lights to the player car
	auto lightEntity = playerCar->addChild();
	auto lc = lightEntity->addComponent<LightingComponent>();
	lc->setSpotLight(glm::vec3(0.94f, 0.89f, 0.54f), glm::vec3(1.f, 0.007f, 0.0002f), glm::radians(20.f), glm::radians(30.f));
	lightEntity->addComponent<TransformComponent>();
	lightEntity->getComponent<TransformComponent>()->setLocalPosition(-0.8f, 0.15f, 1.5f);
	lightEntity->getComponent<TransformComponent>()->setLocalRotation(glm::radians(10.f), glm::vec3(1.f, 0.f, 0.f));

	auto lightEntity2 = playerCar->addChild();
	auto lc2 = lightEntity2->addComponent<LightingComponent>();
	lc2->setSpotLight(glm::vec3(0.94f, 0.89f, 0.54f), glm::vec3(1.f, 0.007f, 0.0002f), glm::radians(20.f), glm::radians(30.f));
	lightEntity2->addComponent<TransformComponent>();
	lightEntity2->getComponent<TransformComponent>()->setLocalPosition(0.8f, 0.15f, 1.5);
	lightEntity2->getComponent<TransformComponent>()->setLocalRotation(glm::radians(10.f), glm::vec3(1.f, 0.f, 0.f));
	//-------------------
	
	playerCar->addComponent<VehicleComponent>();

	auto playerController = playerCar->addComponent<ControllerComponent>();
	playerController->createAxis(GLFW_KEY_W, GLFW_KEY_S, &Events::VehicleAccelerate);
	playerController->createAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, &Events::VehicleAccelerate, ControlAxis::AXIS);
	playerController->createAxis(GLFW_KEY_D, GLFW_KEY_A, &Events::VehicleSteer);
	playerController->createAxis(GLFW_GAMEPAD_AXIS_LEFT_X, &Events::VehicleSteer);
	playerController->bindInput(GLFW_KEY_F, &Events::VehicleFlip);
	playerController->bindInput(GLFW_KEY_LEFT_SHIFT, &Events::VehicleHandbrake);
	playerController->bindInput(GLFW_GAMEPAD_BUTTON_SQUARE, &Events::VehicleHandbrake);

	auto playerAudio = playerCar->addComponent<AudioComponent>();
	playerAudio->addEngineSound("audio/engine.wav", playerCar->getComponent<VehicleComponent>());

	auto playerDescription = playerCar->addComponent<DescriptionComponent>();
	playerDescription->setVec3("Forward", glm::vec3(0.f, 0.f, 1.f));

	collectAINodeVectors();

	// AI nodes
	for (int i = 0; i < aiNodeLocation.size(); i++) {
		gameplay->addAINode(aiNodeType[i], aiNodeArea[i], aiNodeLocation[i]);
	}
	gameplay->testPrintAINodes();

	// --- AI cars ---	
	for (int i = 0; i < g_numAiCars; i++) {
		auto aiCar = scene->addEntity();

		auto tc = aiCar->getComponent<TransformComponent>();
		tc->setLocalRotation(glm::radians(Random::randomFloat(0.0f, 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f));

		// Add wheel entities
		for (int i = 0; i < 4; i++) {
			auto wheel = aiCar->addChild();

			auto mc = wheel->addComponent<ModelComponent>();
			mc->setModel(modelWheel);

			auto rc = wheel->addComponent<RendererComponent>();
			rc->enableRender();

			auto desc = wheel->addComponent<DescriptionComponent>();
			desc->setInteger("wheel", 1);
		}

		auto mc = aiCar->addComponent<ModelComponent>();
		mc->setModel(modelAiCar);

		auto rc = aiCar->addComponent<RendererComponent>();
		rc->enableRender();

		auto lightChild = aiCar->addChild();
		auto Lc = lightChild->addComponent<LightingComponent>();
		Lc->setSpotLight(glm::vec3(0.94f, 0.89f, 0.54f), glm::vec3(1.f, 0.007f, 0.0002f), glm::radians(20.f), glm::radians(30.f));
		lightChild->addComponent<TransformComponent>();
		lightChild->getComponent<TransformComponent>()->setLocalPosition(0.8f, 0.15f, 1.5);
		lightChild->getComponent<TransformComponent>()->setLocalRotation(glm::radians(10.f), glm::vec3(1.f, 0.f, 0.f));

		lightChild = aiCar->addChild();
		Lc = lightChild->addComponent<LightingComponent>();
		Lc->setSpotLight(glm::vec3(0.94f, 0.89f, 0.54f), glm::vec3(1.f, 0.007f, 0.0002f), glm::radians(20.f), glm::radians(30.f));
		lightChild->addComponent<TransformComponent>();
		lightChild->getComponent<TransformComponent>()->setLocalPosition(-0.8f, 0.15f, 1.5);
		lightChild->getComponent<TransformComponent>()->setLocalRotation(glm::radians(10.f), glm::vec3(1.f, 0.f, 0.f));

		aiCar->addComponent<VehicleComponent>();
		aiCar->addComponent<AiComponent>();
		auto audioComponent = aiCar->addComponent<AudioComponent>();
		audioComponent->addSound(AudioTrigger::Collision, "audio/oof.wav");
		aiCars.push_back(aiCar);
	}

	// --- Map road ---
	auto mapRoadTransform = mapRoad->getComponent<TransformComponent>();

	auto mapRoadModel = mapRoad->addComponent<ModelComponent>();
	mapRoadModel->setModel(modelMapRoad);

	auto mapRoadRender = mapRoad->addComponent<RendererComponent>();
	mapRoadRender->enableRender();

	// --- Map Background Grass ---
	auto mapGrassTransform = mapGrass->getComponent<TransformComponent>();

	auto mapGrassModel = mapGrass->addComponent<ModelComponent>();
	mapGrassModel->setModel(modelMapGrass);

	auto mapGrassRender = mapGrass->addComponent<RendererComponent>();
	mapGrassRender->enableRender();

	// --- Map Mall ---
	auto mapMallTransform = mapMall->getComponent<TransformComponent>();
	mapMallTransform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapMallModel = mapMall->addComponent<ModelComponent>();
	mapMallModel->setModel(modelMapMall);

	auto mapMallRender = mapMall->addComponent<RendererComponent>();
	mapMallRender->enableRender();

	auto mapMallRigidbody = mapMall->addComponent<RigidbodyComponent>();
	mapMallRigidbody->addActorStaticMesh(*modelMapMall, convert<physx::PxTransform>(mapMallTransform->getGlobalMatrix()));

	// --- Map Mall Text ---
	auto mapMallTextTransform = mapMallText->getComponent<TransformComponent>();
	mapMallTextTransform->localTranslate(0.0f, -5.5f, 0.0f);

	auto mapMallTextModel = mapMallText->addComponent<ModelComponent>();
	mapMallTextModel->setModel(modelMapMallText);

	auto mapMallTextRender = mapMallText->addComponent<RendererComponent>();
	mapMallTextRender->enableRender();

	// --- Map Boundary Walls ---
	// - Left Wall - 
	auto mapWall1Transform = mapWall1->getComponent<TransformComponent>();
	mapWall1Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall1Model = mapWall1->addComponent<ModelComponent>();
	mapWall1Model->setModel(modelMapWall1);

	auto mapWall1Rigidbody = mapWall1->addComponent<RigidbodyComponent>();
	mapWall1Rigidbody->addActorStaticMesh(*modelMapWall1, convert<physx::PxTransform>(mapWall1Transform->getGlobalMatrix()));

	// - Right Wall - 
	auto mapWall2Transform = mapWall2->getComponent<TransformComponent>();
	mapWall2Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall2Model = mapWall2->addComponent<ModelComponent>();
	mapWall2Model->setModel(modelMapWall2);

	auto mapWall2Rigidbody = mapWall2->addComponent<RigidbodyComponent>();
	mapWall2Rigidbody->addActorStaticMesh(*modelMapWall2, convert<physx::PxTransform>(mapWall2Transform->getGlobalMatrix()));

	// - Back Wall - 
	auto mapWall3Transform = mapWall3->getComponent<TransformComponent>();
	mapWall3Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall3Model = mapWall3->addComponent<ModelComponent>();
	mapWall3Model->setModel(modelMapWall3);

	auto mapWall3Rigidbody = mapWall3->addComponent<RigidbodyComponent>();
	mapWall3Rigidbody->addActorStaticMesh(*modelMapWall3, convert<physx::PxTransform>(mapWall3Transform->getGlobalMatrix()));

	// --- Map Metal Fence ---
	metalFenceLocation = collectGLMVecFromFile("../../res/modelTransformations/metalfenceLocation.txt", metalFenceLocation);
	metalFenceRotation = collectfloatFromFile("../../res/modelTransformations/metalfenceRotation.txt", metalFenceRotation);

	for (int i = 0; i < metalFenceLocation.size(); i++) {
		auto mapMetalFence = scene->addEntity();
		auto mapMetalFenceTransform = mapMetalFence->getComponent<TransformComponent>();
		mapMetalFenceTransform->localTranslate(metalFenceLocation.at(i));
		mapMetalFenceTransform->localRotate(glm::radians(metalFenceRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapMetalFenceModel = mapMetalFence->addComponent<ModelComponent>();
		mapMetalFenceModel->setModel(modelMapMetalFence);

		auto mapMetalFenceRender = mapMetalFence->addComponent<RendererComponent>();
		mapMetalFenceRender->enableRender();
	}

	// --- Map Warning Wall ---
	warningWallLocation = collectGLMVecFromFile("../../res/modelTransformations/warningWallLocation.txt", warningWallLocation);
	warningWallRotation = collectfloatFromFile("../../res/modelTransformations/warningWallRotation.txt", warningWallRotation);

	for (int i = 0; i < warningWallLocation.size(); i++) {
		auto mapwarningWall = scene->addEntity();
		auto mapwarningWallTransform = mapwarningWall->getComponent<TransformComponent>();
		mapwarningWallTransform->localTranslate(warningWallLocation.at(i));
		mapwarningWallTransform->localRotate(glm::radians(warningWallRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapwarningWallModel = mapwarningWall->addComponent<ModelComponent>();
		mapwarningWallModel->setModel(modelMapWarningWall);

		auto mapwarningWallRender = mapwarningWall->addComponent<RendererComponent>();
		mapwarningWallRender->enableRender();
	}

	// --- Map Curb ---
	curbLocation = collectGLMVecFromFile("../../res/modelTransformations/curbLocation.txt", curbLocation);
	curbRotation = collectfloatFromFile("../../res/modelTransformations/curbRotation.txt", curbRotation);

	for (int i = 0; i < curbLocation.size(); i++) {
		auto mapCurb = scene->addEntity();
		auto mapCurbTransform = mapCurb->getComponent<TransformComponent>();
		mapCurbTransform->localTranslate(curbLocation.at(i));
		mapCurbTransform->localRotate(glm::radians(curbRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapCurbModel = mapCurb->addComponent<ModelComponent>();
		mapCurbModel->setModel(modelMapCurb);

		auto mapCurbRender = mapCurb->addComponent<RendererComponent>();
		mapCurbRender->enableRender();

		auto mapCurbRigidbody = mapCurb->addComponent<RigidbodyComponent>();
		mapCurbRigidbody->addActorStaticMesh(*modelMapCurb, convert<physx::PxTransform>(mapCurbTransform->getGlobalMatrix()));
	}

	// --- Map Background Road ---
	roadLocation = collectGLMVecFromFile("../../res/modelTransformations/roadLocation.txt", roadLocation);

	for (int i = 0; i < roadLocation.size(); i++) {
		auto mapBGRoad = scene->addEntity();
		auto mapBGRoadTransform = mapBGRoad->getComponent<TransformComponent>();
		mapBGRoadTransform->localTranslate(roadLocation.at(i));

		auto mapBGRoadModel = mapBGRoad->addComponent<ModelComponent>();
		mapBGRoadModel->setModel(modelMapBGRoad);

		auto mapBGRoadRender = mapBGRoad->addComponent<RendererComponent>();
		mapBGRoadRender->enableRender();
	}

	// --- Map Hedges ---
	hedgeLocation = collectGLMVecFromFile("../../res/modelTransformations/hedgeLocation.txt", hedgeLocation);
	hedgeRotation = collectfloatFromFile("../../res/modelTransformations/hedgeRotation.txt", hedgeRotation);

	for (int i = 0; i < hedgeLocation.size(); i++) {
		auto mapHedge = scene->addEntity();
		auto mapHedgeTransform = mapHedge->getComponent<TransformComponent>();
		mapHedgeTransform->localTranslate(hedgeLocation.at(i));
		mapHedgeTransform->localRotate(glm::radians(hedgeRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapHedgeModel = mapHedge->addComponent<ModelComponent>();
		mapHedgeModel->setModel(modelMapHedge);

		auto mapHedgeRender = mapHedge->addComponent<RendererComponent>();
		mapHedgeRender->enableRender();

		auto mapHedgeRigidbody = mapHedge->addComponent<RigidbodyComponent>();
		mapHedgeRigidbody->addActorStaticBox(physx::PxVec3(7.0f, 1.9f, 0.8f), convert<physx::PxTransform>(mapHedgeTransform->getGlobalMatrix()));
	}

	// --- Map Wood Fence ---
	woodFenceLocation = collectGLMVecFromFile("../../res/modelTransformations/woodfenceLocation.txt", woodFenceLocation);
	woodFenceRotation = collectfloatFromFile("../../res/modelTransformations/woodfenceRotation.txt", woodFenceRotation);

	for (int i = 0; i < woodFenceLocation.size(); i++) {
		auto mapWoodFence = scene->addEntity();
		auto mapWoodFenceTransform = mapWoodFence->getComponent<TransformComponent>();
		mapWoodFenceTransform->localTranslate(woodFenceLocation.at(i));
		mapWoodFenceTransform->localRotate(glm::radians(woodFenceRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapWoodFenceModel = mapWoodFence->addComponent<ModelComponent>();
		mapWoodFenceModel->setModel(modelMapWoodFence);

		auto mapWoodFenceRender = mapWoodFence->addComponent<RendererComponent>();
		mapWoodFenceRender->enableRender();

		auto mapWoodFenceRigidbody = mapWoodFence->addComponent<RigidbodyComponent>();
		mapWoodFenceRigidbody->addActorStaticMesh(*modelMapWoodFence, convert<physx::PxTransform>(mapWoodFenceTransform->getGlobalMatrix()));
	}

	// --- Map Wood Fence Pole ---
	woodFencePoleLocation = collectGLMVecFromFile("../../res/modelTransformations/woodfencePoleLocation.txt", woodFencePoleLocation);
	woodFencePoleRotation = collectfloatFromFile("../../res/modelTransformations/woodfencePoleRotation.txt", woodFencePoleRotation);

	for (int i = 0; i < woodFencePoleLocation.size(); i++) {
		auto mapWoodFencePole = scene->addEntity();
		auto mapWoodFencePoleTransform = mapWoodFencePole->getComponent<TransformComponent>();
		mapWoodFencePoleTransform->localTranslate(woodFencePoleLocation.at(i));
		mapWoodFencePoleTransform->localRotate(glm::radians(woodFencePoleRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapWoodFencePoleModel = mapWoodFencePole->addComponent<ModelComponent>();
		mapWoodFencePoleModel->setModel(modelMapWoodFencePole);

		auto mapWoodFencePoleRender = mapWoodFencePole->addComponent<RendererComponent>();
		mapWoodFencePoleRender->enableRender();

		auto mapWoodFencePoleRigidbody = mapWoodFencePole->addComponent<RigidbodyComponent>();
		mapWoodFencePoleRigidbody->addActorStaticMesh(*modelMapWoodFencePole, convert<physx::PxTransform>(mapWoodFencePoleTransform->getGlobalMatrix()));
	}

	// --- Map Trees ---
	treeLocation = collectGLMVecFromFile("../../res/modelTransformations/treeLocation.txt", treeLocation);

	for (int i = 0; i < treeLocation.size(); i++) {
		// - Tree Stumps - 
		auto mapTreeStump = scene->addEntity();
		auto mapTreeStumpTransform = mapTreeStump->getComponent<TransformComponent>();
		mapTreeStumpTransform->localTranslate(treeLocation.at(i));

		auto mapTreeStumpModel = mapTreeStump->addComponent<ModelComponent>();
		mapTreeStumpModel->setModel(modelMapTreeStump);

		auto mapTreeStumpRender = mapTreeStump->addComponent<RendererComponent>();
		mapTreeStumpRender->enableRender();

		auto mapTreeStumpRigidbody = mapTreeStump->addComponent<RigidbodyComponent>();
		mapTreeStumpRigidbody->addActorStaticMesh(*modelMapTreeStump, convert<physx::PxTransform>(mapTreeStumpTransform->getGlobalMatrix()));

		// - Tree Leaves - 
		auto mapTreeLeaves = scene->addEntity();
		auto mapTreeLeavesTransform = mapTreeLeaves->getComponent<TransformComponent>();
		mapTreeLeavesTransform->localTranslate(treeLocation.at(i));
		mapTreeLeavesTransform->localRotate(Random::randomFloat(glm::radians(0.f), glm::radians(360.f)), glm::vec3(0.f, 1.f, 0.f));

		auto mapTreeLeavesModel = mapTreeLeaves->addComponent<ModelComponent>();
		mapTreeLeavesModel->setModel(modelMapTreeLeaves);

		auto mapTreeLeavesRender = mapTreeLeaves->addComponent<RendererComponent>();
		mapTreeLeavesRender->enableRender();
	}


	// --- Map Parking Lines ---
	parkingLineLocation = collectGLMVecFromFile("../../res/modelTransformations/parkingLinesLocation.txt", parkingLineLocation);
	parkingLineRotation = collectfloatFromFile("../../res/modelTransformations/parkingLinesRotation.txt", parkingLineRotation);

	for(int i = 0; i < parkingLineLocation.size(); i++){
		auto mapParkingLine = scene->addEntity();
		auto mapParkingLineTransform = mapParkingLine->getComponent<TransformComponent>();
		mapParkingLineTransform->localTranslate(parkingLineLocation.at(i));
		mapParkingLineTransform->localRotate(glm::radians(parkingLineRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapParkingLineModel = mapParkingLine->addComponent<ModelComponent>();
		mapParkingLineModel->setModel(modelMapParkingLine);

		auto mapParkingLineRender = mapParkingLine->addComponent<RendererComponent>();
		mapParkingLineRender->enableRender();
	}

	// --- Map Ramps ---
	rampLocation = collectGLMVecFromFile("../../res/modelTransformations/rampLocation.txt", rampLocation);
	rampRotation = collectfloatFromFile("../../res/modelTransformations/rampRotation.txt", rampRotation);

	for(int i = 0; i < rampLocation.size(); i++){
		auto mapRamp = scene->addEntity();
		auto mapRampTransform = mapRamp->getComponent<TransformComponent>();
		//mapRampTransform->localScale(glm::vec3(2.0f, 1.0f, 1.0f));
		mapRampTransform->localTranslate(rampLocation.at(i));
		mapRampTransform->localRotate(glm::radians(rampRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapRampModel = mapRamp->addComponent<ModelComponent>();
		mapRampModel->setModel(modelMapRamp);

		auto mapRampRender = mapRamp->addComponent<RendererComponent>();
		mapRampRender->enableRender();

		auto mapRampRigidbody = mapRamp->addComponent<RigidbodyComponent>();
		mapRampRigidbody->addActorStaticMesh(*modelMapRamp, convert<physx::PxTransform>(mapRampTransform->getGlobalMatrix()));
	}

	// --- Parking Spots ---
	parkingSpotLocation = collectGLMVecFromFile("../../res/modelTransformations/parkingSpotLocation.txt", parkingSpotLocation);
	parkingSpotRotation = collectfloatFromFile("../../res/modelTransformations/parkingSpotRotation.txt", parkingSpotRotation);
	emptySubsetLocation = collectGLMVecFromFile("../../res/modelTransformations/emptySpotSubsetLocation.txt", emptySubsetLocation);
	emptySubsetRotation = collectfloatFromFile("../../res/modelTransformations/emptySpotSubsetRotation.txt", emptySubsetRotation);

	// - Temp Vectors -
	std::vector<glm::vec3> tempSubsetLocation = emptySubsetLocation;
	std::vector<float> tempSubsetRotation = emptySubsetRotation;

	//RANDOMLY PICKS EMPTY PARKING SPACES BY 1 LESS THE NUMBER OF AI + PLAYER
	for(int i = 0; i < /*g_numAiCars*/ 3; i++){
		int spotChoice = rand() % tempSubsetLocation.size();

		//USE THESE VECTORS FOR EMPTY PARKING SPOTS
		emptyParkingSpotLocation.push_back(tempSubsetLocation.at(spotChoice));
		emptyParkingSpotRotation.push_back(tempSubsetRotation.at(spotChoice));

		tempSubsetLocation.erase(tempSubsetLocation.begin() + spotChoice);
		tempSubsetRotation.erase(tempSubsetRotation.begin() + spotChoice);
	}

	// - Propcar Instancing -
	for(int i = 0; i < parkingSpotLocation.size(); i++){
		bool checkSpot = false;
		for(auto & x : emptyParkingSpotLocation){
			if(x == parkingSpotLocation.at(i)){
				checkSpot = true;
			}
		}

		if(!checkSpot){
			int randomNum = rand() % 2 + 1;

			if(randomNum == 1) {
				parkingSpotRotation.at(i) += 180.f;
			}

			auto propCar = scene->addEntity();
			auto propCarTransform = propCar->getComponent<TransformComponent>();

			propCarTransform->localTranslate(parkingSpotLocation.at(i));
			propCarTransform->localRotate(Random::randomFloat(glm::radians(parkingSpotRotation.at(i) - 7.5f), glm::radians(parkingSpotRotation.at(i) + 7.5f)), glm::vec3(0.f, 1.f, 0.f));

			randomNum = rand() % 4 + 1;

			if(randomNum == 1){
				auto propCarModel = propCar->addComponent<ModelComponent>();
				propCarModel->setModel(modelPropCar1);

				auto propCarRender = propCar->addComponent<RendererComponent>();
				propCarRender->enableRender();

				auto propCarRigidbody = propCar->addComponent<RigidbodyComponent>();
				propCarRigidbody->addActorDynamic(*modelPropCar1, convert<physx::PxTransform>(propCarTransform->getGlobalMatrix()));
			} else if(randomNum == 2){
				auto propCarModel = propCar->addComponent<ModelComponent>();
				propCarModel->setModel(modelPropCar2);

				auto propCarRender = propCar->addComponent<RendererComponent>();
				propCarRender->enableRender();

				auto propCarRigidbody = propCar->addComponent<RigidbodyComponent>();
				propCarRigidbody->addActorDynamic(*modelPropCar2, convert<physx::PxTransform>(propCarTransform->getGlobalMatrix()));
			} else if(randomNum == 3){
				auto propCarModel = propCar->addComponent<ModelComponent>();
				propCarModel->setModel(modelPropCar3);

				auto propCarRender = propCar->addComponent<RendererComponent>();
				propCarRender->enableRender();

				auto propCarRigidbody = propCar->addComponent<RigidbodyComponent>();
				propCarRigidbody->addActorDynamic(*modelPropCar3, convert<physx::PxTransform>(propCarTransform->getGlobalMatrix()));
			} else if(randomNum == 4){
				auto propCarModel = propCar->addComponent<ModelComponent>();
				propCarModel->setModel(modelPropCar4);

				auto propCarRender = propCar->addComponent<RendererComponent>();
				propCarRender->enableRender();

				auto propCarRigidbody = propCar->addComponent<RigidbodyComponent>();
				propCarRigidbody->addActorDynamic(*modelPropCar4, convert<physx::PxTransform>(propCarTransform->getGlobalMatrix()));
			}
		}
	}

	// --- Test ----
	for(int i = 0; i < emptyParkingSpotLocation.size(); i++){
		auto testparkingspace = scene->addEntity();
		auto testparkingspaceTransform = testparkingspace->getComponent<TransformComponent>();

		testparkingspaceTransform->localTranslate(emptyParkingSpotLocation.at(i));
		testparkingspaceTransform->localRotate(glm::radians(emptyParkingSpotRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		testparkingspace->addComponent<RendererComponent>();
		testparkingspace->getComponent<RendererComponent>()->enableRender();
		testparkingspace->getComponent<RendererComponent>()->enableTransparentRendering();
		testparkingspace->addComponent<ModelComponent>();
		testparkingspace->getComponent<ModelComponent>()->setModel(modelMapParkingIndicator);
	}

	// --- TriggerBox for Parking Space ---
	auto triggerBoxComponent = triggerBox->addComponent<VolumeTriggerComponent>();
	for (int i = 0; i < emptyParkingSpotLocation.size(); i++) {
		triggerBoxComponent->createVolumeShape(PxTransform(PxVec3(emptyParkingSpotLocation.at(i).x, emptyParkingSpotLocation.at(i).y, emptyParkingSpotLocation.at(i).z)), PxBoxGeometry(1.f, 1.f, 1.f));
	};

	/* --------------------- End Game World Description --------------------- */

	// Hacky stuff
	playerId = playerCar->id();
	scores[playerId] = 0;

	for (auto aiCar : aiCars) {
		gameplay->addAiId(aiCar->id());
		scores[aiCar->id()] = 0;
	}

	audio->setListener(mainCamera->getComponent<TransformComponent>());
}



Application::~Application() {
	//Clean-up
	glfwTerminate();
}

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

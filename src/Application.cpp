#include "Application.h"
#include <Random.h>

unsigned int playerId = 0;
std::vector<unsigned int> aiList;
std::unordered_map<unsigned int, int> scores;

glm::vec3 parkingVertices[] = {
	//GROUP 1 - Horizontal Parking Spaces
	//NEEDS 90 or 270 degree rotation
	glm::vec3(28.5f, 2.0f, 42.8f),
	glm::vec3(28.5f, 2.0f, 36.2f),
	glm::vec3(28.5f, 2.0f, 29.6f),
	glm::vec3(28.5f, 2.0f, 23.0f),
	glm::vec3(28.5f, 2.0f, 16.4f),
	glm::vec3(28.5f, 2.0f, 9.8f),
	glm::vec3(28.5f, 2.0f, 3.2f),
	glm::vec3(28.5f, 2.0f, -3.4f),
	glm::vec3(28.5f, 2.0f, -10.0f),
	glm::vec3(28.5f, 2.0f, -16.6f),
	glm::vec3(28.5f, 2.0f, -23.2f),
	glm::vec3(28.5f, 2.0f, -46.2f),
	glm::vec3(-148.5f, 2.0f, 49.6f),
	glm::vec3(-148.5f, 2.0f, 29.6f),
	glm::vec3(-148.5f, 2.0f, 23.0f),
	glm::vec3(-148.5f, 2.0f, 16.4f),
	glm::vec3(-148.5f, 2.0f, 9.8f),
	glm::vec3(-148.5f, 2.0f, 3.2f),
	glm::vec3(-148.5f, 2.0f, -3.4f),
	glm::vec3(-148.5f, 2.0f, -10.0f),
	glm::vec3(-148.5f, 2.0f, -16.6f),
	glm::vec3(-148.5f, 2.0f, -29.8f),
	glm::vec3(-148.5f, 2.0f, -36.4f),
	glm::vec3(-148.5f, 2.0f, -43.0f),
	glm::vec3(149.7f, 2.0f, 42.8f),
	glm::vec3(149.7f, 2.0f, 36.2f),
	glm::vec3(149.7f, 2.0f, 29.6f),
	glm::vec3(149.7f, 2.0f, 23.0f),
	glm::vec3(149.7f, 2.0f, 16.4f),
	glm::vec3(149.7f, 2.0f, 9.8f),
	glm::vec3(149.7f, 2.0f, 3.2f),
	glm::vec3(149.7f, 2.0f, -3.4f),
	glm::vec3(149.7f, 2.0f, -10.0f),
	glm::vec3(149.7f, 2.0f, -16.6f),
	glm::vec3(149.7f, 2.0f, -23.2f),
	glm::vec3(149.7f, 2.0f, -46.2f),
	glm::vec3(-27.7f, 2.0f, 49.6f),
	glm::vec3(-27.7f, 2.0f, 29.6f),
	glm::vec3(-27.7f, 2.0f, 23.0f),
	glm::vec3(-27.7f, 2.0f, 16.4f),
	glm::vec3(-27.7f, 2.0f, 9.8f),
	glm::vec3(-27.7f, 2.0f, 3.2f),
	glm::vec3(-27.7f, 2.0f, -3.4f),
	glm::vec3(-27.7f, 2.0f, -10.0f),
	glm::vec3(-27.7f, 2.0f, -16.6f),
	glm::vec3(-27.7f, 2.0f, -23.2f),
	glm::vec3(-27.7f, 2.0f, -46.2f),

	//GROUP 2 - Vertical Parking Spaces
	glm::vec3(39.8f, 2.0f, 61.0f),
	glm::vec3(46.4f, 2.0f, 61.0f),
	glm::vec3(53.0f, 2.0f, 61.0f),
	glm::vec3(59.6f, 2.0f, 61.0f),
	glm::vec3(66.2f, 2.0f, 61.0f),
	glm::vec3(72.8f, 2.0f, 61.0f),
	glm::vec3(79.4f, 2.0f, 61.0f),
	glm::vec3(99.2f, 2.0f, 61.0f),
	glm::vec3(105.8f, 2.0f, 61.0f),
	glm::vec3(112.4f, 2.0f, 61.0f),
	glm::vec3(119.0f, 2.0f, 61.0f),
	glm::vec3(125.6f, 2.0f, 61.0f),
	glm::vec3(132.2f, 2.0f, 61.0f),
	glm::vec3(138.8f, 2.0f, 61.0f),
	glm::vec3(39.8f, 2.0f, -57.0f),
	glm::vec3(53.0f, 2.0f, -57.0f),
	glm::vec3(59.6f, 2.0f, -57.0f),
	glm::vec3(66.2f, 2.0f, -57.0f),
	glm::vec3(72.8f, 2.0f, -57.0f),
	glm::vec3(79.4f, 2.0f, -57.0f),
	glm::vec3(99.2f, 2.0f, -57.0f),
	glm::vec3(105.8f, 2.0f, -57.0f),
	glm::vec3(112.4f, 2.0f, -57.0f),
	glm::vec3(119.0f, 2.0f, -57.0f),
	glm::vec3(125.6f, 2.0f, -57.0f),
	glm::vec3(132.2f, 2.0f, -57.0f),
	glm::vec3(138.8f, 2.0f, -57.0f),
	glm::vec3(-38.6f, 2.0f, 61.0f),
	glm::vec3(-51.8f, 2.0f, 61.0f),
	glm::vec3(-58.4f, 2.0f, 61.0f),
	glm::vec3(-65.0f, 2.0f, 61.0f),
	glm::vec3(-71.6f, 2.0f, 61.0f),
	glm::vec3(-78.2f, 2.0f, 61.0f),
	glm::vec3(-84.8f, 2.0f, 61.0f),
	glm::vec3(-91.4f, 2.0f, 61.0f),
	glm::vec3(-98.0f, 2.0f, 61.0f),
	glm::vec3(-104.6f, 2.0f, 61.0f),
	glm::vec3(-111.2f, 2.0f, 61.0f),
	glm::vec3(-117.8f, 2.0f, 61.0f),
	glm::vec3(-124.4f, 2.0f, 61.0f),
	glm::vec3(-131.0f, 2.0f, 61.0f),
	glm::vec3(-137.6f, 2.0f, 61.0f),
	glm::vec3(-38.6f, 2.0f, -57.0f),
	glm::vec3(-45.2f, 2.0f, -57.0f),
	glm::vec3(-51.8f, 2.0f, -57.0f),
	glm::vec3(-58.4f, 2.0f, -57.0f),
	glm::vec3(-65.0f, 2.0f, -57.0f),
	glm::vec3(-71.6f, 2.0f, -57.0f),
	glm::vec3(-78.2f, 2.0f, -57.0f),
	glm::vec3(-98.0f, 2.0f, -57.0f),
	glm::vec3(-104.6f, 2.0f, -57.0f),
	glm::vec3(-111.2f, 2.0f, -57.0f),
	glm::vec3(-117.8f, 2.0f, -57.0f),
	glm::vec3(-124.4f, 2.0f, -57.0f),
	glm::vec3(-131.0f, 2.0f, -57.0f),
	glm::vec3(-137.6f, 2.0f, -57.0f)
};

glm::vec3 emptyparkingVertices[] = {
	glm::vec3(-148.5f, -0.9f, -23.0f),
	glm::vec3(46.4f, -0.9f, -57.0f),
	glm::vec3(-45.2f, -0.9f, 61.0f),
};

glm::vec3 rockPosVertices[] = {
	glm::vec3(14.0f, -1.0f, -50.0f),
	glm::vec3(11.0f, -1.0f, -56.0f),
	glm::vec3(20.0f, -1.0f, -64.0f),
	glm::vec3(17.0f, -1.0f, -55.0f),
	glm::vec3(-21.0f, -1.0f, -56.0f),
	glm::vec3(-17.0f, -1.0f, -62.0f),
	glm::vec3(-9.0f, -1.0f, -54.0f),
	glm::vec3(14.0f, -1.0f, 50.0f),
	glm::vec3(13.0f, -1.0f, 56.0f),
	glm::vec3(16.0f, -1.0f, 64.0f),
	glm::vec3(-14.0f, -1.0f, 55.0f),
	glm::vec3(-7.0f, -1.0f, 62.0f),
	glm::vec3(-17.0f, -1.0f, 53.0f),
	glm::vec3(-9.0f, -1.0f, 58.0f)
};

Application::Application(appSettings& settings): 
	settings(settings)
{
	//App initialization
	glfwInit();
	Time::init();
	Random::init();
	/* Framework - used by systems*/
	scene    = Scene::newScene();
	window   = std::make_shared<Window>(1200, 800, "Test Window");

	/* Game systems - update() every frame */
	input    = std::make_shared<InputSystem>(window);
	gameplay = std::make_shared<GameplaySystem>(scene);
	physics  = std::make_shared<PhysicsSystem>(scene);
	render   = std::make_shared<RenderSystem>(scene, window);
	audio    = std::make_shared<AudioSystem>();


	/* --------------------- Game World Description ------------------------ */

	// --- Entities ---
	auto playerCar = scene->addEntity();
	auto aiCar = scene->addEntity();

	auto mapGrass = scene->addEntity();
	auto mapRoad = scene->addEntity();
	auto mapLines = scene->addEntity();
	auto mapWall1 = scene->addEntity();
	auto mapWall2 = scene->addEntity();
	auto mapWall3 = scene->addEntity();
	auto mapWall4 = scene->addEntity();

	auto triggerBox = scene->addEntity();

	auto environmentalLight = scene->addEntity();
	auto mainCamera = playerCar->addChild();
	auto shadowCamera = playerCar->addChild();


	// --- Models ---
	auto modelPlayerCar = std::make_shared<Model>(
		"models/car1.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelAiCar = std::make_shared<Model>(
		"models/car2.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelPropCar = std::make_shared<Model>(
		"models/car3.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelMapGrass = std::make_shared<Model>(
		"models/gamemapGrassPlane.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapRoad = std::make_shared<Model>(
		"models/gamemapRoad.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapLines = std::make_shared<Model>(
		"models/gamemapParkingLines.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapWall1 = std::make_shared<Model>(
		"models/gamemapBrickWall1.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapWall2 = std::make_shared<Model>(
		"models/gamemapBrickWall2.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapWall3 = std::make_shared<Model>(
		"models/gamemapBrickWall3.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapWall4 = std::make_shared<Model>(
		"models/gamemapBrickWall4.obj", glm::vec3(.5f, .1f, .2f));

	auto modelParkIndic = std::make_shared<Model>(
		"models/gamemapParkGuide.obj", glm::vec3(.5f, .1f, .2f));

	auto modelRock = std::make_shared<Model>(
		"models/Rock1.obj", glm::vec3(0.0f, 1.0f, 1.0f));


	// --- Directional light ---
	environmentalLight->addComponent<LightingComponent>();
	environmentalLight->getComponent<LightingComponent>()->setAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
	environmentalLight->getComponent<LightingComponent>()->setDirectionalLight(glm::vec3(0.7f, 0.7f, 0.7f));


	// --- Main camera ---
	auto mainCameraTransform = mainCamera->getComponent<TransformComponent>();
	mainCameraTransform->setLocalPosition(0.0f, 9.0f, -5.5f);
	mainCameraTransform->setLocalRotation(glm::radians(-45.0f), glm::vec3(1.f, 0.f, 0.f));
	mainCameraTransform->localRotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate to face the other way
	
	auto mainCamerCam = mainCamera->addComponent<CameraComponent>();
	mainCamerCam->setPerspectiveCamera(glm::radians(90.f), 1.f /*Will be modified to the window*/,0.01f, 300.f);


	// --- Shadow map camera ---
	auto shadowCameraTransform = shadowCamera->getComponent<TransformComponent>();
	auto q1 = physx::PxQuat(glm::radians(-60.f), physx::PxVec3(1.f, 0.f, 0.f));
	auto q2 = physx::PxQuat(glm::radians(-90.f), physx::PxVec3(0.f, 1.f, 0.f));
	shadowCameraTransform->setLocalPosition(-30.0f, 20.0f, 0.0f);
	shadowCameraTransform->setLocalRotation(q2 * q1);

	auto shadowCameraCam = shadowCamera->addComponent<CameraComponent>();
	shadowCameraCam->setOrthographicCamera(100.f, 100.f, 0.1f, 100.f);

	auto shadowCameraDesc = shadowCamera->addComponent<DescriptionComponent>();
	shadowCameraDesc->setInteger("Ignore parent rotations in render", 1);

	
	// --- Player car ---	
	auto playerTransform = playerCar->getComponent<TransformComponent>();
	
	auto playerModel = playerCar->addComponent<ModelComponent>();
	playerModel->setModel(modelPlayerCar);
	
	auto playerRender = playerCar->addComponent<RendererComponent>();
	playerRender->enableRender();
	
	auto playerVehicle = playerCar->addComponent<VehicleComponent>();
	
	auto playerController = playerCar->addComponent<ControllerComponent>();
	playerController->createAxis(GLFW_KEY_W, GLFW_KEY_S, &Events::VehicleAccelerate);
	playerController->createAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, &Events::VehicleAccelerate, ControlAxis::AXIS);
	playerController->createAxis(GLFW_KEY_D, GLFW_KEY_A, &Events::VehicleSteer);
	playerController->createAxis(GLFW_GAMEPAD_AXIS_LEFT_X, &Events::VehicleSteer);
	playerController->bindInput(GLFW_KEY_F, &Events::GameReset);
	playerController->bindInput(GLFW_KEY_LEFT_SHIFT, &Events::VehicleHandbrake);
	playerController->bindInput(GLFW_GAMEPAD_BUTTON_SQUARE, &Events::VehicleHandbrake);



	// --- AI car ---	
	auto aiCarTransform = aiCar->getComponent<TransformComponent>();
	aiCarTransform->setLocalPosition(0.0f, 5.0f, 12.0f);
	aiCarTransform->setLocalRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	auto aiCarModel = aiCar->addComponent<ModelComponent>();
	aiCarModel->setModel(modelAiCar);
	
	auto aiCarRender = aiCar->addComponent<RendererComponent>();
	aiCarRender->enableRender();
	
	auto aiCarVehicle = aiCar->addComponent<VehicleComponent>();
	
	auto aiCarAI = aiCar->addComponent<AiComponent>();

	// --- Prop car ---
	for(int i = 0; i < sizeof(parkingVertices)/sizeof(*parkingVertices); i++){
		auto propCar = scene->addEntity();
		auto propCarTransform = propCar->getComponent<TransformComponent>();
		propCarTransform->setLocalPosition(parkingVertices[i]);
		
		if(i < 47){
			int randomRotate = rand() % 10;
			if(randomRotate <= 4) {
				propCarTransform->setLocalRotation(Random::randomFloat(glm::radians(80.f), glm::radians(100.f)), glm::vec3(0.f, 1.f, 0.f));	
			} else {
				propCarTransform->setLocalRotation(Random::randomFloat(glm::radians(260.f), glm::radians(280.f)), glm::vec3(0.f, 1.f, 0.f));
			}
		} else {
			int randomRotate = rand() % 10;
			if(randomRotate <= 4) {
				propCarTransform->setLocalRotation(Random::randomFloat(glm::radians(170.f), glm::radians(190.f)), glm::vec3(0.f, 1.f, 0.f));	
			} else {
				propCarTransform->setLocalRotation(Random::randomFloat(glm::radians(-10.f), glm::radians(10.f)), glm::vec3(0.f, 1.f, 0.f));	
			}
		}

		auto propCarModel = propCar->addComponent<ModelComponent>();
		propCarModel->setModel(modelPropCar);

		auto propCarRender = propCar->addComponent<RendererComponent>();
		propCarRender->enableRender();
		
		auto propCarRigidbody = propCar->addComponent<RigidbodyComponent>();
		propCarRigidbody->addActorDynamic(*modelPropCar, convert<physx::PxTransform>(propCarTransform->getGlobalMatrix()));
	}

	

	// --- Map grass ---
	auto mapGrassTransform = mapGrass->getComponent<TransformComponent>();
	mapGrassTransform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapGrassModel = mapGrass->addComponent<ModelComponent>();
	mapGrassModel->setModel(modelMapGrass);

	auto mapGrassRender = mapGrass->addComponent<RendererComponent>();
	mapGrassRender->enableRender();


	// --- Map road ---
	auto mapRoadTransform = mapRoad->getComponent<TransformComponent>();
	mapRoadTransform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapRoadModel = mapRoad->addComponent<ModelComponent>();
	mapRoadModel = mapRoad->getComponent<ModelComponent>();
	mapRoadModel->setModel(modelMapRoad);

	auto mapRoadRender = mapRoad->addComponent<RendererComponent>();
	mapRoadRender->enableRender();
	

	// --- Map lines ---
	auto mapLinesTransform = mapLines->getComponent<TransformComponent>();
	mapLinesTransform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapLinesModel = mapLines->addComponent<ModelComponent>();
	mapLinesModel->setModel(modelMapLines);

	auto mapLinesRender = mapLines->addComponent<RendererComponent>();
	mapLinesRender->enableRender();

	// --- Map Walls ---
	auto mapWall1Transform = mapWall1->getComponent<TransformComponent>();
	mapWall1Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall1Model = mapWall1->addComponent<ModelComponent>();
	mapWall1Model->setModel(modelMapWall1);

	auto mapWall1Render = mapWall1->addComponent<RendererComponent>();
	mapWall1Render->enableRender();

	auto mapWall1Rigidbody = mapWall1->addComponent<RigidbodyComponent>();
	mapWall1Rigidbody->addActorStaticMesh(*modelMapWall1, convert<physx::PxTransform>(mapWall1Transform->getGlobalMatrix()));

	
	auto mapWall2Transform = mapWall2->getComponent<TransformComponent>();
	mapWall2Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall2Model = mapWall2->addComponent<ModelComponent>();
	mapWall2Model->setModel(modelMapWall2);

	auto mapWall2Render = mapWall2->addComponent<RendererComponent>();
	mapWall2Render->enableRender();

	auto mapWall2Rigidbody = mapWall2->addComponent<RigidbodyComponent>();
	mapWall2Rigidbody->addActorStaticMesh(*modelMapWall2, convert<physx::PxTransform>(mapWall2Transform->getGlobalMatrix()));


	auto mapWall3Transform = mapWall3->getComponent<TransformComponent>();
	mapWall3Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall3Model = mapWall3->addComponent<ModelComponent>();
	mapWall3Model->setModel(modelMapWall3);

	auto mapWall3Render = mapWall3->addComponent<RendererComponent>();
	mapWall3Render->enableRender();

	auto mapWall3Rigidbody = mapWall3->addComponent<RigidbodyComponent>();
	mapWall3Rigidbody->addActorStaticMesh(*modelMapWall3, convert<physx::PxTransform>(mapWall3Transform->getGlobalMatrix()));


	auto mapWall4Transform = mapWall4->getComponent<TransformComponent>();
	mapWall4Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall4Model = mapWall4->addComponent<ModelComponent>();
	mapWall4Model->setModel(modelMapWall4);

	auto mapWall4Render = mapWall4->addComponent<RendererComponent>();
	mapWall4Render->enableRender();

	auto mapWall4Rigidbody = mapWall4->addComponent<RigidbodyComponent>();
	mapWall4Rigidbody->addActorStaticMesh(*modelMapWall4, convert<physx::PxTransform>(mapWall4Transform->getGlobalMatrix()));

	// --- Map Guide ---
	for(int i = 0; i < sizeof(emptyparkingVertices)/sizeof(*emptyparkingVertices); i++){
		auto ParkIndic = scene->addEntity();
		auto ParkIndicTransform = ParkIndic->getComponent<TransformComponent>();
		ParkIndicTransform->setLocalPosition(emptyparkingVertices[i]);
		if(i == 0){
			ParkIndicTransform->setLocalRotation(glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		} else if(i == 2) {
			ParkIndicTransform->setLocalRotation(glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
		}

		auto ParkIndicModel = ParkIndic->addComponent<ModelComponent>();
		ParkIndicModel->setModel(modelParkIndic);

		auto ParkIndicRender = ParkIndic->addComponent<RendererComponent>();
		ParkIndicRender->enableRender();
	}
	
	// --- Rocks ---
	sp<ModelComponent>     rockModel     = nullptr;
	sp<RendererComponent>  rockRender    = nullptr;
	sp<TransformComponent> rockTransform = nullptr;
	sp<RigidbodyComponent> rockRigidbody = nullptr;

	for (int i = 0; i < sizeof(rockPosVertices)/sizeof(*rockPosVertices); i++) {
		auto rock = scene->addEntity();
		rockTransform = rock->getComponent<TransformComponent>();
		rockModel = rock->addComponent<ModelComponent>();
		rockRender = rock->addComponent<RendererComponent>();
		rockRigidbody = rock->addComponent<RigidbodyComponent>();

		rockModel->setModel(modelRock);
		rockRender->enableRender();
		rockTransform->setLocalPosition(rockPosVertices[i]);
		rockTransform->setLocalRotation(Random::randomFloat(glm::radians(0.0f), glm::radians(360.0f)), glm::vec3(0.f, 1.f, 0.f));
	
		rockRigidbody->addActorStaticSphere(0.6f, convert<physx::PxTransform>(rockTransform->getGlobalMatrix()));
	}

	
	// --- TriggerBox ---
	auto triggerBoxComponent = triggerBox->addComponent<VolumeTriggerComponent>();
	for (int i = 0; i < sizeof(emptyparkingVertices) / sizeof(*emptyparkingVertices); i++) {
		if(emptyparkingVertices[i].x == -148.5f){
			triggerBoxComponent->createVolumeShape(PxTransform(PxVec3(emptyparkingVertices[i].x, 0.0f, emptyparkingVertices[i].z)), PxBoxGeometry(1.0f, 1.0f, 1.0f));
		}

		else {
			triggerBoxComponent->createVolumeShape(PxTransform(PxVec3(emptyparkingVertices[i].x, 0.0f, emptyparkingVertices[i].z)), PxBoxGeometry(1.0f, 1.0f, 1.0f));
		}
		
	};

	// Hacky stuff
	playerId = playerCar->id();
	scores[playerId] = 0;
	gameplay->addAiId(aiCar->id());

	audio->setListener(mainCamera->getComponent<TransformComponent>());
	audio->car = playerCar->getComponent<VehicleComponent>()->vehicle->getRigidDynamicActor();


	/* --------------------- End Game World Description --------------------- */




}

int Application::play() {

	// Invoke observers of the GameStart event
	Events::GameStart.broadcast();

	//Game loop
	while (!window->shouldClose()) {
		
		// Process input
		input->processInput();

		// Update time-related values
		Time::update();
	
		// Fixed time step game loop
		while (Time::takeNextStep()) {
			if (scores[playerId] >= 5 || scores[aiList[0]] >= 5) {
			}
			else {
				gameplay->update();	// Gameplay / AI update
				physics->update();	// Physics update
				audio->update();	// Audio update
			}
			


		}

		// Render the current scene
		render->update();

	}

	return 0;
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

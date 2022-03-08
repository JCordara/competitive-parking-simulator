#include "Application.h"
#include <Random.h>
unsigned int g_boxID;
unsigned int g_carID;
unsigned int g_aiID;

Application::Application(appSettings& settings): 
	settings(settings)
{
	//App initialization
	glfwInit();
	Time::init();
	Random::init();
	/* Framework - used by systems*/
	scene        = Scene::newScene();
	window 		 = std::make_shared<Window>(1200, 800, "Test Window");

	/* Game systems - update() every frame */
	input        = std::make_shared<InputSystem>(window);
	gameplay     = std::make_shared<GameplaySystem>(scene);
	physics      = std::make_shared<PhysicsSystem>(scene);
	render       = std::make_shared<RenderSystem>(scene, window);
	audio        = std::make_shared<AudioSystem>();


	/* --------------------- Game World Description ------------------------ */

	// --- Models ---
	auto modelPlayerCar = std::make_shared<Model>(
		"models/car1.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelAiCar = std::make_shared<Model>(
		"models/car2.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelPropCar = std::make_shared<Model>(
		"models/car3.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelMapGrass = std::make_shared<Model>(
		"models/gamemapGrassPlane.obj", glm::vec3(.5f, .1f, .2f)
	);

	auto modelMapRoad = std::make_shared<Model>(
		"models/gamemapRoad.obj", glm::vec3(.5f, .1f, .2f)
	);

	auto modelMapLines = std::make_shared<Model>(
		"models/gamemapParkingLines.obj", glm::vec3(.5f, .1f, .2f));

	auto modelRock = std::make_shared<Model>(
		"models/Rock1.obj", glm::vec3(0.0f, 1.0f, 1.0f));


	// --- Directional light ---
	auto environmentalLight = scene->addEntity();
	environmentalLight->addComponent<LightingComponent>();
	environmentalLight->getComponent<LightingComponent>()->setAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
	environmentalLight->getComponent<LightingComponent>()->setDirectionalLight(glm::vec3(0.7f, 0.7f, 0.7f));


	// --- Prop car ---
	auto propCar = scene->addEntity();
	g_boxID = propCar->id();
	
	auto propCarTransform = propCar->getComponent<TransformComponent>();
	propCarTransform->setLocalPosition(28.5f, 4.0f, 22.5f);
	propCarTransform->setLocalRotation(glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

	auto propCarModel = propCar->addComponent<ModelComponent>();
	propCarModel->setModel(modelPropCar);

	auto propCarRender = propCar->addComponent<RendererComponent>();
	propCarRender->enableRender();
	
	auto propCarRigidbody = propCar->addComponent<RigidbodyComponent>();
	propCarRigidbody->addActorDynamic(
		*modelPropCar, 
		PxTransform(
			propCarTransform->getGlobalPosition().x, 
			propCarTransform->getGlobalPosition().y, 
			propCarTransform->getGlobalPosition().z, 
			propCarTransform->getLocalRotation()
		)
	);

	propCar->addComponent<AudioComponent>();

	
	// --- AI car ---
	auto aiCar = scene->addEntity();
	g_aiID = aiCar->id();
	
	auto aiCarTransform = aiCar->getComponent<TransformComponent>();
	aiCarTransform->setLocalPosition(0.0f, 5.0f, 12.0f);
	aiCarTransform->setLocalRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	auto aiCarModel = aiCar->addComponent<ModelComponent>();
	aiCarModel->setModel(modelAiCar);
	
	auto aiCarRender = aiCar->addComponent<RendererComponent>();
	aiCarRender->enableRender();
	
	auto aiCarVehicle = aiCar->addComponent<VehicleComponent>();
	
	auto aiCarAI = aiCar->addComponent<AiComponent>();


	// --- Player car ---
	auto playerCar = scene->addEntity();
	g_carID = playerCar->id();
	
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
	playerController->bindInput(GLFW_KEY_LEFT_SHIFT, &Events::VehicleHandbrake);
	playerController->bindInput(GLFW_GAMEPAD_BUTTON_SQUARE, &Events::VehicleHandbrake);


	// --- Main camera ---
	auto mainCamera = playerCar->addChild();
	
	auto mainCamerCam = mainCamera->addComponent<CameraComponent>();
	mainCamerCam->setPerspectiveCamera(glm::radians(90.f), 1.f /*Will be modified to the window*/,0.01f, 300.f);

	auto mainCameraTransform = mainCamera->getComponent<TransformComponent>();
	mainCameraTransform->setLocalPosition(0.0f, 9.0f, -5.5f);
	mainCameraTransform->setLocalRotation(glm::radians(-45.0f), glm::vec3(1.f, 0.f, 0.f));
	mainCameraTransform->localRotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate to face the other way


	// --- Shadow map camera ---
	auto shadowCamera = playerCar->addChild();

	auto shadowCameraTransform = shadowCamera->getComponent<TransformComponent>();
	auto q1 = physx::PxQuat(glm::radians(-60.f), physx::PxVec3(1.f, 0.f, 0.f));
	auto q2 = physx::PxQuat(glm::radians(-90.f), physx::PxVec3(0.f, 1.f, 0.f));
	shadowCameraTransform->setLocalPosition(-30.0f, 20.0f, 0.0f);
	shadowCameraTransform->setLocalRotation(q2 * q1);

	auto shadowCameraCam = shadowCamera->addComponent<CameraComponent>();
	shadowCameraCam->setOrthographicCamera(100.f, 100.f, 0.1f, 100.f);

	auto shadowCameraDesc = shadowCamera->addComponent<DescriptionComponent>();
	shadowCameraDesc->setInteger("Ignore parent rotations in render", 1);


	// --- Map grass ---
	auto mapGrass = scene->addEntity();

	auto mapGrassTransform = mapGrass->getComponent<TransformComponent>();
	mapGrassTransform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapGrassModel = mapGrass->addComponent<ModelComponent>();
	mapGrassModel->setModel(modelMapGrass);

	auto mapGrassRender = mapGrass->addComponent<RendererComponent>();
	mapGrassRender->enableRender();


	// --- Map road ---
	auto mapRoad = scene->addEntity();

	auto mapRoadTransform = mapRoad->getComponent<TransformComponent>();
	mapRoadTransform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapRoadModel = mapRoad->addComponent<ModelComponent>();
	mapRoadModel = mapRoad->getComponent<ModelComponent>();
	mapRoadModel->setModel(modelMapRoad);

	auto mapRoadRender = mapRoad->addComponent<RendererComponent>();
	mapRoadRender->enableRender();
	

	// --- Map lines ---
	auto mapLines = scene->addEntity();

	auto mapLinesTransform = mapLines->getComponent<TransformComponent>();
	mapLinesTransform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapLinesModel = mapLines->addComponent<ModelComponent>();
	mapLinesModel->setModel(modelMapLines);

	auto mapLinesRender = mapLines->addComponent<RendererComponent>();
	mapLinesRender->enableRender();

	
	// --- Rocks ---
	sp<ModelComponent> rockModel = nullptr;
	sp<RendererComponent> rockRender = nullptr;
	sp<TransformComponent> rockTransform = nullptr;
	sp<RigidbodyComponent> rockRigidbody = nullptr;

	for (int i = 0; i < 10; i++) {
		auto rock = scene->addEntity();
		rockTransform = rock->getComponent<TransformComponent>();
		rockModel = rock->addComponent<ModelComponent>();
		rockRender = rock->addComponent<RendererComponent>();
		rockRigidbody = rock->addComponent<RigidbodyComponent>();

		rockModel->setModel(modelRock);
		rockRender->enableRender();
		rockTransform->setLocalPosition(Random::randomVec3(-20.f, 20.f, -1.f, -1.f, -20.f, 20.f));
		rockRigidbody->addActorStatic(*modelRock, convert<physx::PxTransform>(rockTransform->getGlobalMatrix()));
	}

	carTransform = playerCar->getComponent<TransformComponent>();
	mainCamTransform = mainCamera->getComponent<TransformComponent>();
	/* --------------------- End Game World Description --------------------- */

/* 
	// Some random crap Keaton was doing
	std::vector<PxVec3> meshVerts = {
		PxVec3(0 ,1, 0), PxVec3(1, 0, 0), PxVec3(-1, 0, 0), 
		PxVec3(0 ,0, 1), PxVec3(0, 0,-1) 
	};

	std::vector<PxVec3> meshVerts2 = { 
		PxVec3(3, 1,-1), PxVec3(1, 2, 0), PxVec3(-1, 1, 0), 
		PxVec3(0, 1, 1), PxVec3(1, 0,-1), PxVec3( 1, 9,-1), 
		PxVec3(1, 2,-1), PxVec3(1, 2,-1), PxVec3(-1,-1,-1) 
	};

	//Create Mesh Object
	auto mesh = scene->addEntity();
	
	Model newModel("models/car1.obj", glm::vec3(1.0, 1.0, 1.0));
	PxTransform position = PxTransform(PxVec3(0, 20.0f, 0.0f));
	PxTransform position1 = PxTransform(PxVec3(10.0, 0.0f, 0.0f));

	auto rigidbodyComponent = mesh->addComponent<RigidbodyComponent>();
	rigidbodyComponent->addActorDynamic(newModel, position);
	rigidbodyComponent->addActorStatic(newModel, position1);
*/

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

			gameplay->update();	// Gameplay / AI update
			physics->update();	// Physics update
			/*for (int i = 0; i < scene->directChildren().size(); i++) {
				std::cout << scene->directChildren().at(i)
					<< scene->directChildren().at(i)->getComponent<TransformComponent>()->getGlobalPosition()
					<< std::endl;
			}*/

			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 front = glm::vec3(
				-carTransform->getLocalRotation().getBasisVector2().x,
				-carTransform->getLocalRotation().getBasisVector2().y,
				-carTransform->getLocalRotation().getBasisVector2().z
			);
			audio->setListenerOrientation(front, up);
			audio->setListenerPosition(mainCamTransform->getGlobalPosition());
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

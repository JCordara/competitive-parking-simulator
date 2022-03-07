#include "Application.h"

unsigned int g_boxID;
unsigned int g_carID;

Application::Application(appSettings& settings): 
	settings(settings)
{
	//App initialization
	glfwInit();
	Time::init();

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

	auto environmentalLight = scene->addEntity();
	environmentalLight->addComponent<LightingComponent>();
	environmentalLight->getComponent<LightingComponent>()->setAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
	environmentalLight->getComponent<LightingComponent>()->setDirectionalLight(glm::vec3(0.7f, 0.7f, 0.7f));

	auto cameraChild = environmentalLight->addChild();
	cameraChild->addComponent<CameraComponent>();
	cameraChild->getComponent<CameraComponent>()->setOrthographicCamera(100.f, 100.f, 0.1f, 100.f);
	auto transformComponent = cameraChild->getComponent<TransformComponent>();
	transformComponent->setLocalPosition(-15.0f, 7.0f, 0.0f);
	auto q1 = physx::PxQuat(glm::radians(-30.f), physx::PxVec3(1.f, 0.f, 0.f));
	auto q2 = physx::PxQuat(glm::radians(-90.f), physx::PxVec3(0.f, 1.f, 0.f));
	transformComponent->setLocalRotation(q2 * q1);

	auto cube = scene->addEntity();
	auto modelComponent     = cube->addComponent<ModelComponent>();
	auto renderComponent    = cube->addComponent<RendererComponent>();
	auto rigidbodyComponent = cube->addComponent<RigidbodyComponent>();
	cube->addComponent<AudioComponent>();
	transformComponent = cube->getComponent<TransformComponent>();

	g_boxID = cube->id();

	auto cubeModel = std::make_shared<Model>(
		"models/car3.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto playerVehicleModel = std::make_shared<Model>(
		"models/car1.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto aiVehicleModel = std::make_shared<Model>(
		"models/car2.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	modelComponent->setModel(cubeModel);
	renderComponent->enableRender();
	transformComponent->setLocalPosition(1.f, 1.5f, -5.f);
	transformComponent->setLocalRotation(glm::radians(15.f), glm::vec3(0.f, 0.f, 1.f));
	
	rigidbodyComponent->addActorDynamic(
		*cubeModel, 
		PxTransform(
			transformComponent->getGlobalPosition().x, 
			transformComponent->getGlobalPosition().y, 
			transformComponent->getGlobalPosition().z, 
			transformComponent->getLocalRotation()
		)
	);

	auto aiCar = scene->addEntity();
	auto aiCarTransform = aiCar->getComponent<TransformComponent>();
	auto aiCarModel = aiCar->addComponent<ModelComponent>();
	auto aiCarRender = aiCar->addComponent<RendererComponent>();
	auto aiCarVehicle = aiCar->addComponent<VehicleComponent>();
	auto aiCarAI = aiCar->addComponent<AiComponent>();
	aiCarTransform->setLocalPosition(0.0f, 5.0f, 12.0f);
	aiCarTransform->setLocalRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	aiCarModel->setModel(aiVehicleModel);
	aiCarRender->enableRender();

	auto car = scene->addEntity();
	auto carTransform  = car->getComponent<TransformComponent>();
	auto carModel      = car->addComponent<ModelComponent>();
	auto carRender     = car->addComponent<RendererComponent>();
	auto carVehicle    = car->addComponent<VehicleComponent>();
	auto carController = car->addComponent<ControllerComponent>();

	carModel->setModel(playerVehicleModel);
	carRender->enableRender();
	carController->createAxis(GLFW_KEY_W, GLFW_KEY_S, &Events::VehicleAccelerate);
	carController->createAxis(GLFW_KEY_A, GLFW_KEY_D, &Events::VehicleSteer);
	carController->bindInput(GLFW_KEY_LEFT_SHIFT, &Events::VehicleHandbrake);

	g_carID = car->id();

	auto camera = car->addChild();
	camera->addComponent<CameraComponent>();
	camera->getComponent<CameraComponent>()->setPerspectiveCamera(glm::radians(90.f), 1.f /*Will be modified to the window*/,0.01f, 300.f);
	auto camTransform = camera->getComponent<TransformComponent>();
	camTransform->setLocalPosition(0.0f, 9.0f, -5.5f);
	camTransform->setLocalRotation(glm::radians(-45.0f), glm::vec3(1.f, 0.f, 0.f));
	camTransform->localRotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate to face the other way

	mainCamTransform = camera->getComponent<TransformComponent>();

	auto mapGrass = scene->addEntity();
	mapGrass->addComponent<ModelComponent>();
	mapGrass->addComponent<RendererComponent>();

	auto mapGrassModel = std::make_shared<Model>(
		"models/gamemapGrassPlane.obj", glm::vec3(.5f, .1f, .2f)
	);

	modelComponent = mapGrass->getComponent<ModelComponent>();
	modelComponent->setModel(mapGrassModel);
	renderComponent = mapGrass->getComponent<RendererComponent>();
	renderComponent->enableRender();
	transformComponent = mapGrass->getComponent<TransformComponent>();
	transformComponent->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapRoad = scene->addEntity();
	mapRoad->addComponent<ModelComponent>();
	mapRoad->addComponent<RendererComponent>();

	auto mapRoadModel = std::make_shared<Model>(
		"models/gamemapRoad.obj", glm::vec3(.5f, .1f, .2f)
	);

	modelComponent = mapRoad->getComponent<ModelComponent>();
	modelComponent->setModel(mapRoadModel);
	renderComponent = mapRoad->getComponent<RendererComponent>();
	renderComponent->enableRender();
	transformComponent = mapRoad->getComponent<TransformComponent>();
	transformComponent->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapPLines = scene->addEntity();
	mapPLines->addComponent<ModelComponent>();
	mapPLines->addComponent<RendererComponent>();

	auto mapPLinesModel = std::make_shared<Model>(
		"models/gamemapParkingLines.obj", glm::vec3(.5f, .1f, .2f)
	);

	modelComponent = mapPLines->getComponent<ModelComponent>();
	modelComponent->setModel(mapPLinesModel);
	renderComponent = mapPLines->getComponent<RendererComponent>();
	renderComponent->enableRender();
	transformComponent = mapPLines->getComponent<TransformComponent>();
	transformComponent->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapFence = scene->addEntity();
	mapFence->addComponent<ModelComponent>();
	mapFence->addComponent<RendererComponent>();

	auto mapFenceModel = std::make_shared<Model>(
		"models/gamemapBoundaryFence.obj", glm::vec3(.5f, .5f, .5f)
	);

	modelComponent = mapFence->getComponent<ModelComponent>();
	modelComponent->setModel(mapFenceModel);
	renderComponent = mapFence->getComponent<RendererComponent>();
	renderComponent->enableRender();
	transformComponent = mapFence->getComponent<TransformComponent>();
	transformComponent->localTranslate(0.0f, -1.0f, 0.0f);

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
			glm::vec3 front = glm::vec3(
				mainCamTransform->getLocalRotation().getBasisVector2().x,
				mainCamTransform->getLocalRotation().getBasisVector2().y,
				mainCamTransform->getLocalRotation().getBasisVector2().z
			);
			glm::vec3 up = glm::vec3(
				mainCamTransform->getLocalRotation().getBasisVector1().x,
				mainCamTransform->getLocalRotation().getBasisVector1().y,
				mainCamTransform->getLocalRotation().getBasisVector1().z
			);

			// printf("Front: (%.1f, %.1f, %.1f)\n", front.x, front.y, front.z);
			// printf("Up: (%.1f, %.1f, %.1f)\n\n", up.x, up.y, up.z);

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

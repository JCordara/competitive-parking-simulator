#include "Application.h"

// Milestone hacks
void Application::vroomSound(Entity&, float) {
	audioSystem->createSource().playAudio(audioSystem->loadAudio("audio/rev.wav"));
}

void Application::collisionSound() {
	audioSystem->createSource().playAudio(audioSystem->loadAudio("audio/oof.wav"));
}

// Actual code
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
	inputSystem  = std::make_shared<InputSystem>(window);
	gameplay     = std::make_shared<GameplaySystem>(scene);
	physics      = std::make_shared<PhysicsSystem>(scene);
	render       = std::make_shared<RenderSystem>(scene, window);
	audioSystem  = std::make_shared<AudioSystem>();


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
	transformComponent = cube->getComponent<TransformComponent>();

	auto cubeModel = std::make_shared<Model>(
		"models/Test1.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto vehicleModel = std::make_shared<Model>(
		"models/car1.obj", glm::vec3(1.0f, 1.0f, 1.0f));

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
	aiCarAI->setCurrentNode(gameplay->aiGlobalNodes);
	aiCarAI->pickRandGoalNode(gameplay->aiGlobalNodes);
	aiCarModel->setModel(vehicleModel);
	aiCarRender->enableRender();

	auto car = scene->addEntity();
	auto carTransform  = car->getComponent<TransformComponent>();
	auto carModel      = car->addComponent<ModelComponent>();
	auto carRender     = car->addComponent<RendererComponent>();
	auto carVehicle    = car->addComponent<VehicleComponent>();
	auto carController = car->addComponent<ControllerComponent>();

	carModel->setModel(vehicleModel);
	carRender->enableRender();
	carController->createAxis(GLFW_KEY_W, GLFW_KEY_S, &Events::VehicleAccelerate);
	carController->createAxis(GLFW_KEY_A, GLFW_KEY_D, &Events::VehicleSteer);
	carController->bindInput(GLFW_KEY_LEFT_SHIFT, &Events::VehicleHandbrake);

	auto camera = car->addChild();
	camera->addComponent<CameraComponent>();
	camera->getComponent<CameraComponent>()->setPerspectiveCamera(glm::radians(90.f), 1.f /*Will be modified to the window*/,0.01f, 300.f);
	auto camTransform = camera->getComponent<TransformComponent>();
	camTransform->setLocalPosition(0.0f, 10.0f, -5.0f);
	camTransform->setLocalRotation(glm::radians(-45.0f), glm::vec3(1.f, 0.f, 0.f));
	camTransform->localRotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate to face the other way

	auto mapGrass = scene->addEntity();
	mapGrass->addComponent<ModelComponent>();
	mapGrass->addComponent<RendererComponent>();

	auto mapGrassModel = std::make_shared<Model>(
		"models/gamemapWholeMap.obj", glm::vec3(.5f, .1f, .2f)
	);

	modelComponent = mapGrass->getComponent<ModelComponent>();
	modelComponent->setModel(mapGrassModel);
	renderComponent = mapGrass->getComponent<RendererComponent>();
	renderComponent->enableRender();
	transformComponent = mapGrass->getComponent<TransformComponent>();
	transformComponent->localTranslate(0.0f, -1.0f, 0.0f);
	transformComponent->localScale(0.3f, 0.3f, 0.3f);

	Events::CarBoxCollision.registerHandler<Application, &Application::collisionSound>(this);
	Events::VehicleAccelerate.registerHandler<Application, &Application::vroomSound>(this);

	/*auto plane = scene->addEntity();
	plane->addComponent<ModelComponent>();
	plane->addComponent<RendererComponent>();
	plane->addComponent<TransformComponent>();

	auto planeModel = std::make_shared<Model>(
		"models/SmilePlane.obj", glm::vec3(.5f, .1f, .2f));

	modelComponent = plane->getComponent<ModelComponent>();
	modelComponent->setModel(planeModel);
	renderComponent = plane->getComponent<RendererComponent>();
	renderComponent->enableRender();
	transformComponent = plane->getComponent<TransformComponent>();
	*/
	//transformComponent->setLocalPosition(0.f, 0.f, 0.f);
	//transformComponent->setLocalScale(10.f, 1.f, 10.f);
	//transformComponent->setLocalRotation(-acosf(0.4f), glm::vec3(1.f, 0.f, 0.f));

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
		inputSystem->processInput();

		// Update time-related values
		Time::update();
	
		// Fixed time step game loop
		while (Time::takeNextStep()) {

			gameplay->update();	// Gameplay / AI update
			physics->update();	// Physics update

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

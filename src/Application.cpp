#include "Application.h"
#include <PhysX/PxPhysicsAPI.h>

int g_carsParked = 0;
void carParked() {
	g_carsParked++;
}
void carUnParked() {
	//g_carsParked--;
}

Application::Application(appSettings& settings) 
	: settings(settings)
{
	//App initialization
	glfwInit();
	Time::init();

	/* Framework (Managers) - used by systems*/
	scene        = Scene::newScene();
	window 		 = std::make_shared<Window>(1200, 800, "Test Window");
	inputManager = std::make_shared<InputManager>(window);

	/* Game systems - update() every frame */
	gameplay     = std::make_shared<GameplaySystem>(scene);
	physics      = std::make_shared<PhysicsSystem>(scene);
	render       = std::make_shared<RenderSystem>(scene, window);
	audioSystem  = std::make_shared<AudioSystem>();

	/* --------------------- Game World Description ------------------------ */

	auto camera = scene->addEntity();
	camera->addComponent<CameraComponent>();
	camera->getComponent<CameraComponent>()->setPerspectiveCamera(glm::radians(90.f), 1.f /*Will be modified to the window*/,0.01f, 300.f);
	auto transformComponent = camera->getComponent<TransformComponent>();
	transformComponent->setLocalPosition(0.0f, 10.0f, 0.0f);
	transformComponent->setLocalRotation(-atan(5.f / 3.f), glm::vec3(1.f, 0.f, 0.f));

	auto environmentalLight = scene->addEntity();
	environmentalLight->addComponent<LightingComponent>();
	environmentalLight->getComponent<LightingComponent>()->setAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
	environmentalLight->getComponent<LightingComponent>()->setDirectionalLight(glm::vec3(0.7f, 0.7f, 0.7f));

	auto cameraChild = environmentalLight->addChild();
	cameraChild->addComponent<CameraComponent>();
	cameraChild->getComponent<CameraComponent>()->setOrthographicCamera(100.f, 100.f, 0.1f, 100.f);
	transformComponent = cameraChild->getComponent<TransformComponent>();
	transformComponent->setLocalPosition(-15.0f, 7.0f, 0.0f);
	auto q1 = physx::PxQuat(glm::radians(-30.f), physx::PxVec3(1.f, 0.f, 0.f));
	auto q2 = physx::PxQuat(glm::radians(-90.f), physx::PxVec3(0.f, 1.f, 0.f));
	transformComponent->setLocalRotation(q2 * q1);

	auto cube = scene->addEntity();
	cube->addComponent<ModelComponent>();
	cube->addComponent<RendererComponent>();
	cube->addComponent<TransformComponent>();

	auto cubeModel = std::make_shared<Model>(
		"models/Test1.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelComponent = cube->getComponent<ModelComponent>();
	modelComponent->setModel(cubeModel);
	auto renderComponent = cube->getComponent<RendererComponent>();
	renderComponent->enableRender();
	transformComponent = cube->getComponent<TransformComponent>();
	transformComponent->setLocalPosition(1.f, 1.5f, -5.f);
	transformComponent->setLocalScale(3.f, 1.f, 1.f);
	transformComponent->setLocalRotation(glm::radians(15.f), glm::vec3(0.f, 0.f, 1.f));
	
	auto mapGrass = scene->addEntity();
	mapGrass->addComponent<ModelComponent>();
	mapGrass->addComponent<RendererComponent>();
	mapGrass->addComponent<TransformComponent>();

	auto mapGrassModel = std::make_shared<Model>(
		"models/gamemapWholeMap.obj", glm::vec3(.5f, .1f, .2f)
	);

	modelComponent = mapGrass->getComponent<ModelComponent>();
	modelComponent->setModel(mapGrassModel);
	renderComponent = mapGrass->getComponent<RendererComponent>();
	renderComponent->enableRender();
	transformComponent = mapGrass->getComponent<TransformComponent>();

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

}

int Application::play() {

	// Invoke observers of the GameStart event
	Events::GameStart.broadcast();

	//Game loop
	while (!window->shouldClose()) {
		
		// Process input
		inputManager->processInput();

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

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
	audioManager = std::make_shared<AudioManager>();

	/* Game systems - update() every frame */
	gameplay     = std::make_shared<GameplaySystem>(scene, audioManager);
	physics      = std::make_shared<PhysicsSystem>(scene, audioManager);
	render       = std::make_shared<RenderSystem>(scene, audioManager, window);


	/* --------------------- Game World Description ------------------------ */

	auto camera = scene->addEntity();
	camera->addComponent<CameraComponent>();
	camera->getComponent<CameraComponent>()->setPerspectiveCamera(glm::radians(90.f), 1.f /*Will be modified to the window*/,0.01f, 300.f);
	auto transformComponent = camera->getComponent<TransformComponent>();
	transformComponent->setLocalPosition(0.0f, 5.0f, .0f);
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
	transformComponent->setLocalPosition(1.f, 1.f, -5.f);
	transformComponent->setLocalScale(3.f, 1.f, 1.f);
	transformComponent->setLocalRotation(glm::radians(15.f), glm::vec3(0.f, 0.f, 1.f));
	
	auto plane = scene->addEntity();
	plane->addComponent<ModelComponent>();
	plane->addComponent<RendererComponent>();
	plane->addComponent<TransformComponent>();

	auto planeModel = std::make_shared<Model>(
		"models/smileplane.obj", glm::vec3(.5f, .1f, .2f));

	modelComponent = plane->getComponent<ModelComponent>();
	modelComponent->setModel(planeModel);
	renderComponent = plane->getComponent<RendererComponent>();
	renderComponent->enableRender();
	transformComponent = plane->getComponent<TransformComponent>();
	transformComponent->setLocalPosition(0.f, -2.f, 0.f);
	transformComponent->setLocalScale(10.f, 1.f, 10.f);
	//transformComponent->setLocalRotation(-acosf(0.4f), glm::vec3(1.f, 0.f, 0.f));
	
	

	/* --------------------- End Game World Description --------------------- */

	/* ---------------------------------------------------------------------- */

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

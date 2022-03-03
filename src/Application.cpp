#include "Application.h"

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
	camera->getComponent<CameraComponent>()->setPurpose(CameraPurpose::render);
	camera->getComponent<TransformComponent>()->setPosition(0.0f, 0.0f, 3.0f);

	auto ambientLight = scene->addEntity();
	ambientLight->addComponent<LightingComponent>();
	ambientLight->getComponent<LightingComponent>()->getAmbient()->setCol(glm::vec3(0.1f, 0.1f, 0.1f));

	auto directionalLight = scene->addEntity();
	directionalLight->addComponent<LightingComponent>();
	directionalLight->getComponent<LightingComponent>()->getDirectionalLight()->setCol(glm::vec3(0.7f, 0.7f, 0.7f));

	auto cameraChild = directionalLight->addChild();
	cameraChild->addComponent<CameraComponent>();
	cameraChild->getComponent<CameraComponent>()->setPurpose(CameraPurpose::shadowMap);
	cameraChild->getComponent<TransformComponent>()->setPosition(0.0f, 1.0f, 3.0f);

	auto cube = scene->addEntity();
	cube->addComponent<ModelComponent>();
	cube->addComponent<RendererComponent>();

	auto cubeModel = std::make_shared<Model>(
		"res/models/Test1.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelComponent = cube->getComponent<ModelComponent>();
	modelComponent->setModel(cubeModel);

	/* --------------------- End Game World Description --------------------- */

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

#include "Application.h"

Application::Application(appSettings& settings) : settings(settings) {
	//App initialization
	glfwInit();

	/* Framework (Managers) - used by systems*/
	scene  		 = std::make_shared<Scene>();
	eventManager = std::make_shared<GameEventManager>();
	audioManager = std::make_shared<AudioManager>();
	window 		 = std::make_shared<Window>(eventManager, 1200, 800, "Test Window");

	/* Game systems - update() every frame */
	gameplay = std::make_shared<GameplaySystem>(scene, eventManager, audioManager);
	physics  = std::make_shared<PhysicsSystem>(scene, eventManager, audioManager);
	render   = std::make_shared<RenderSystem>(scene, eventManager, audioManager, window);
}

int Application::play() {
	//Game loop
	while (!window->shouldClose()) {
		// Get current input events
		glfwPollEvents();

		// Update time-related values (fps, frame delta, etc.)
		Time::update();

		gameplay->update();
		physics->update();
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

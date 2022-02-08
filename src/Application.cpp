#include "Application.h"

Application::Application(appSettings& settings) : settings(settings) {
	//App initialization
}

int Application::play() {
	//Game loop
	/*
	while (!window.shouldClose()) {
		// Get current input events
		glfwPollEvents();

		// Update time-related values (fps, frame delta, etc.)
		Time::update();

		gameplay.update();
		physics.update();
		render.update();

	}
	*/
	return 0;
}

Application::~Application() {
	//Clean-up
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

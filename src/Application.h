#pragma once
#include <string>
// C++ standard library includes
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <limits>
#include <math.h>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Engine sub-system includes
#include "Application.h"
#include "Scene.h"
#include "Application.h"

#include "Entity.h"
#include "Scene.h"
#include "Event.h"
#include "AudioSystem.h"
#include "TimeInfo.h"
#include "Renderers.h"

// Other stuff
#include "GameCallback.h"
#include "Log.h"
#include "GLDebug.h"
#include "GUI.h"
#include "Window.h"
#include "Shader.h"
#include "EditorCamera.h"

struct appSettings {
	std::string version;
};


class Application {
public:
	Application(appSettings& settings);
	int play();
	~Application();

	Application() = delete;
	Application(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator= (const Application&) = delete;
	Application& operator= (Application&&) = delete;

private:
	appSettings settings;

	std::shared_ptr<Window> window;
	std::shared_ptr<Scene> scene;
	std::shared_ptr<GameLogicSystem> gameLogicSystem;
	std::shared_ptr<PhysicsSystem> physicsSystem;
	std::shared_ptr<RenderSystem> renderSystem;
	std::shared_ptr<AudioSystem> audioSystem;
	std::shared_ptr<GameEventManager> eventManager;
	//Any collections and/or managers need to be here

};

//Functions to load settings
appSettings defaultSettings();
appSettings loadSettings(std::string filepath);

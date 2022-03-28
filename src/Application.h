#pragma once

// Build glew library statically
#define GLEW_STATIC

// Use header only FMT library to avoid unnecessary shared libraries
#define FMT_HEADER_ONLY

// For math functions
#define _USE_MATH_DEFINES

// External library includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <PhysX/PxPhysicsAPI.h>
#include <stack>
#include <queue>

// C++ standard library includes
#include "Common.h"

// Engine sub-system includes
#include "Framework.h"
#include "GameplaySystem.h"
#include "Physics/PhysicsSystem.h"
#include "RenderSystem.h"
#include "Input/InputSystem.h"
#include "Audio/AudioSystem.h"

// Other stuff
#include "Renderers.h"
#include "GLDebug.h"
#include "GuiScene.h"
#include "Shader.h"
#include "EditorCamera.h"
#include "Window.h"

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
	void gameStart();
	void gameClose();
	void gameOpenOptions();
	void roundWonMenu();

private:

	appSettings settings;

	/* Framework */
	shared_ptr<Scene>          scene;
	shared_ptr<GuiScene>       guiScene;
	shared_ptr<Window>         window;

	/* Game systems - update() every frame */
	shared_ptr<GameplaySystem> gameplay;
	shared_ptr<PhysicsSystem>  physics;
	shared_ptr<RenderSystem>   render;
	shared_ptr<AudioSystem>    audio;
	shared_ptr<InputSystem>    input;

	std::stack<shared_ptr<Menu>> menuStack;
	std::queue<shared_ptr<Scene>> sceneQueue;
	void setupBaseLevelGUI();
	void setupMainMenu();
	void setupBaseLevel(shared_ptr<Scene> scene);
};

//Functions to load settings
appSettings defaultSettings();
appSettings loadSettings(std::string filepath);

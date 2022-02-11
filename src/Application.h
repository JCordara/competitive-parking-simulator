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

// C++ standard library includes
#include "Common.h"

// Engine sub-system includes
#include "Framework.h"
#include "GameplaySystem.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"

// Other stuff
#include "Renderers.h"
#include "GLDebug.h"
#include "GUI.h"
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

	/* Framework (Managers) - used by systems*/
	std::shared_ptr<GameEventManager>  eventManager;
	std::shared_ptr<AudioManager>      audioManager;
	std::shared_ptr<Scene>             scene;
	std::shared_ptr<Window>            window;

	/* Game systems - update() every frame */
	std::shared_ptr<GameplaySystem>    gameplay;
	std::shared_ptr<PhysicsSystem>     physics;
	std::shared_ptr<RenderSystem>      render;

	// Rendering stuff (temporary location until rendering system is done)
	glm::mat4 identity;
	EditorCamera mainCamera;
	std::shared_ptr<GameRenderPipeline> renderPipeline;
	std::vector<std::shared_ptr<PointLight>> scenePointLights;
	std::vector< std::shared_ptr<SpotLight>> sceneSpotLights;
	std::shared_ptr<DirectionalLight> dirLight;
	Camera directionalLightCamera;
	std::shared_ptr<AmbientLight> ambientLight;
	std::vector<std::shared_ptr<Model>> sceneRenderModels;
	std::vector<GameObject> sceneCubeGameObjects;
	std::vector<GameObject> CarObjects;
	std::vector<GameObject> scenePlaneGameObjects;
	std::shared_ptr<GUI> gui;

	float viewportAspectRatio;
};

//Functions to load settings
appSettings defaultSettings();
appSettings loadSettings(std::string filepath);

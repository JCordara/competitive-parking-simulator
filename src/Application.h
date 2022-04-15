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
#include "Event.h"

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
	void exitApplication();
	~Application();

	Application() = delete;
	Application(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator= (const Application&) = delete;
	Application& operator= (Application&&) = delete;
	
private:
	/* --- Framework --- */
	shared_ptr<Scene>          scene;
	shared_ptr<GuiScene>       guiScene;
	shared_ptr<Window>         window;
	shared_ptr<Menu>		   menu;
	bool					   playgame;
	/* Game systems - update() (at least) every frame */
	shared_ptr<GameplaySystem> gameplay;
	shared_ptr<PhysicsSystem>  physics;
	shared_ptr<RenderSystem>   render;
	shared_ptr<AudioSystem>    audio;
	shared_ptr<InputSystem>    input;
	/* --- Imgui Textures --- */
	std::vector<std::pair<string, std::shared_ptr<Texture>>> imguiTextures;
	/* --- Loaded Data --- */
	std::vector<std::pair<string, std::shared_ptr<Model>>> loadedModels;
	std::vector<std::pair<string, std::vector<instancedTransformation>>> loadedInstancedTransformations;
	std::vector<std::shared_ptr<AiGraphNode>> aiGraph;
	appSettings settings;
	void loadModels();
	void loadImguiTextures();
	void loadImguiTexture(std::string path, GLint interpolation, bool flip);
	void loadModel(string filename, glm::vec3 col);
	void loadInstancedTransformations(string modelKey, string positionTransformation, string axisAngleRotationTransformation);
	std::optional<std::vector<instancedTransformation>> retrieveInstancedTransformations(string name);
	std::vector<instancedTransformation> getInstancedTransformationsOrThrow(string name);
	std::optional<std::shared_ptr<Texture>> retrieveImguiTexture(string name);
	std::shared_ptr<Texture> getImguiTextureOrThrow(string name);
	std::optional <std::shared_ptr<Model>> retrieveModel(string name);
	std::shared_ptr<Model> getModelOrThrow(string name);
	/* --- Level manipulators --- */
	void generateStaticMap();
	void createCar(string chassisModelName, std::shared_ptr<Entity> ent);
	std::shared_ptr<Entity> createPlayerEntity(instancedTransformation transformation);
	std::shared_ptr<Entity> addAICar(string alias);
	std::shared_ptr<Entity> addDynamicObect(string alias, string modelName, instancedTransformation transformation);
	std::shared_ptr<Entity> addTriggerBoxEntity(string alias, string modelName, instancedTransformation transformation, PxBoxGeometry boxgeom);
	/* --- Entity Manipulation Events --- */
	void addOpenParkingEntity(string alias, instancedTransformation transformation);
	void addPropCar(string alias, instancedTransformation transformation);
	void addAICarEvent(string alias);
	/* --- GUI Handlers --- */
	void setupMainMenu();
	void setupGameMenu();
	void setupOptions();
	void infoMenu();
	void controlsMenu();
	void setupBaseLevelGUI();
	void roundWonMenu();
	void gameEndGui(string message);
};

//Functions to load settings
appSettings defaultSettings();
appSettings loadSettings(std::string filepath);
vector<glm::vec3> collectGLMVecFromFile(string filepath, vector<glm::vec3> vec);
vector<float> collectfloatFromFile(string filepath, vector<float> vec);


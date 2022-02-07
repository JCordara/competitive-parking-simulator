/** Some kind of big comment to document stuff probably goes here
* Authors, license, disclaimer, whatever
*/

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


#define TRACKBALL_CAM	1	// Use a trackball dev cam


class PlayCarSound {
public:
	AudioSource& car;
	Audio& sound;
	PlayCarSound(AudioSource& c, Audio& s) : car(c), sound(s) {
		// Respond to TestAudio events
		Events::TestAudio.registerHandler<PlayCarSound, &PlayCarSound::playVROOOOOOM>(this);

		// Respond to TestAudioParameter events
		// When TestAudioParameter events are broadcast, they come with an audio
		// buffer parameter to play
		Events::TestAudioParameter.registerHandler<PlayCarSound, &PlayCarSound::playSound>(this);
	}
	void playSound(Audio& buf) {
		car.setGain(0.5f);
		car.playAudio(buf);
	}
	void playVROOOOOOM() {
		car.setGain(1.0f);
		car.playAudio(sound);
	}
};


//-------------------------------
int main() {

	Log::debug("Starting main");

	// Create and initialize game systems
	auto eventManager = std::make_shared<GameEventManager>();
	auto audioSystem  = std::make_shared<AudioSystem>();
	auto scene = std::make_shared<Scene>();

	// WINDOW
	glfwInit();
	Window window(1200, 800, "Test Window");
	window.setCallbacks(eventManager);

	// --------------------------- Example Audio -------------------------------
	// Use references to ensure proper memory deallocation
	Audio& sound = audioSystem->loadAudio("audio/rev.wav");
	Audio& defaultSound = audioSystem->loadAudio("audio/null.wav");
	// Can also create a non-static source
	// Non-static sources will need their positions updated each frame
	AudioSource& carAudio = audioSystem->createStaticSource(glm::vec3(0.0f, 0.0f, 0.0f));
	// -------------------------------------------------------------------------

	// ---------------- Example Keyboard Event Registration --------------------
	// Command to play vroom sound encapsulated in an object
	PlayCarSound testPlayer = PlayCarSound(carAudio, sound);
	
	// Register S key to broadcast a TestAudio event
	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::TestAudio), 
		GLFW_KEY_S, GLFW_PRESS, 0
	);
	// -------------------------------------------------------------------------
	

	// ------------------ Example Entity and Component Usage --------------------
	Entity frank;
	frank.addComponent<TransformComponent>();
	std::shared_ptr<TransformComponent> transform;
	transform = frank.getComponent<TransformComponent>();
	transform->x = 420.0f;
	std::cout << "Frank's red hot transform component:\nPosition: ("
		<< transform->x << ", "
		<< transform->y << ", "
		<< transform->z << ")\n";

	printf("%d\n", frank.hasComponent<TransformComponent>());
	printf("%d\n", frank.hasComponent<PhysicsComponent>());
	frank.removeComponent<TransformComponent>();
	
	// Outputs error message and returns nullptr
	// transform = frank.getComponent<TransformComponent>();
	// -------------------------------------------------------------------------

	// screw gldebug
	// GLDebug::enable();
	glm::mat4 identity(1.0f);

	// ------------------------------ Cameras ----------------------------------
#if TRACKBALL_CAM
	EditorCamera mainCamera = EditorCamera();
	mainCamera.setPitch(-35.0);
	mainCamera.setYaw(-45.0);
	mainCamera.setZoom(5.5);
	mainCamera.setLookAt(glm::vec3(0.0f, 0.0f, -1.0f));

	// Register camera input events
	eventManager->registerMouseButton(
		bindMethodFunction_0_Variables(&EditorCamera::button1Down, &mainCamera), 
		GLFW_MOUSE_BUTTON_1, GLFW_PRESS, 0
	);

	eventManager->registerMouseButton(
		bindMethodFunction_0_Variables(&EditorCamera::button1Up, &mainCamera), 
		GLFW_MOUSE_BUTTON_1, GLFW_RELEASE, 0
	);

	eventManager->registerMouseButton(
		bindMethodFunction_0_Variables(&EditorCamera::button2Down, &mainCamera), 
		GLFW_MOUSE_BUTTON_2, GLFW_PRESS, 0
	);
	
	eventManager->registerMouseButton(
		bindMethodFunction_0_Variables(&EditorCamera::button2Up, &mainCamera), 
		GLFW_MOUSE_BUTTON_2, GLFW_RELEASE, 0
	);

	eventManager->registerMousePosition(
		bindMethodFunction_2_Variables(&EditorCamera::move, &mainCamera)
	);

	eventManager->registerMousePosition(
		bindMethodFunction_2_Variables(&EditorCamera::rotateAroundTarget, &mainCamera)
	);

	eventManager->registerScroll(
		bindMethodFunction_2_Variables(&EditorCamera::zoom, &mainCamera)
	);

#else
	Camera mainCamera = Camera(glm::vec3(2.0f,2.5f,0.0f), glm::radians(45.0f), glm::radians(-45.0f), glm::radians(100.f), 3.f / 2.f, 0.01f, 1000.0f);
	SizeTogglerTest toggleSize = SizeTogglerTest(eventManager, mainCamera);
	eventManager->registerKey(bindMethodFunction_0_Variables(&SizeTogglerTest::enableWindowUpdate, &toggleSize), GLFW_KEY_T, GLFW_RELEASE, 0);
	eventManager->registerKey(bindMethodFunction_0_Variables(&SizeTogglerTest::disableWindowUpdate, &toggleSize), GLFW_KEY_T, GLFW_PRESS, 0);
#endif
	// -------------------------------------------------------------------------
	auto renderPipeline = std::make_shared<GameRenderPipeline>();
	//-----Lights
	std::vector<std::shared_ptr<PointLight>> scenePointLights = {
		std::make_shared<PointLight>(glm::vec3(0.5f, 1.0f, 1.0f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 0.5f, 0.01f)),
		std::make_shared<PointLight>(glm::vec3(1.0f, 2.0f, -0.5f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.1f, 0.5f))
	};

	std::vector< std::shared_ptr<SpotLight>> sceneSpotLights = {
		std::make_shared<SpotLight>(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 0.01f, 0.00f), glm::normalize(glm::vec3(0.0f, -1.f, 0.0f)), glm::radians(12.0f), glm::radians(40.0f)),
		std::make_shared<SpotLight>(glm::vec3(-1.5f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 0.01f, 0.00f), glm::normalize(glm::vec3(-1.f, -1.f, 0.f)), glm::radians(30.0f), glm::radians(55.0f))
	};
	std::shared_ptr<DirectionalLight> dirLight = std::make_shared<DirectionalLight>(0.3f*glm::vec3(1.f, 1.f, 1.f), glm::normalize(glm::vec3(0.0f, -1.0f, 1.0f)));
	Camera directionalLightCamera = Camera(glm::vec3(0.0f, 15.0f, -15.0f), glm::radians(180.0f), glm::radians(-45.0f), 100.0f, 50.f, 5.f, 50.f, true);
	std::shared_ptr<AmbientLight> ambientLight = std::make_shared<AmbientLight>(0.05f * glm::vec3(1.0f, 1.0f, 1.0f));

	for (auto light : scenePointLights) renderPipeline->addPointLight(light);
	for (auto light : sceneSpotLights) renderPipeline->addSpotLight(light);
	renderPipeline->setDirectionalLight(dirLight);
	renderPipeline->setAmbientLight(ambientLight);

	//-----Models
	std::vector<std::shared_ptr<Model>> sceneRenderModels = {
		std::make_shared<Model>("models/Test1.obj", glm::vec3(1.0, 1.0, 1.0)),
		std::make_shared<Model>("models/Test2_M.obj", glm::vec3(1.0, 0.0, 1.0))
	};

	//------Objects
	std::vector<GameObject> sceneCubeGameObjects = {
		GameObject(0,-1, glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f))),
		GameObject(0,-1, glm::scale(glm::translate(identity, glm::vec3(0.0f, 0.0f, 6.0f)), glm::vec3(1.0f, 2.0f, 0.5f))),
		GameObject(0,-1, (glm::scale(glm::rotate(glm::translate(identity, glm::vec3(6.0f, 1.0f, 6.0f)),glm::radians(30.f),glm::vec3(0.f,1.f,0.f)), glm::vec3(1.0f, 1.0f, 1.0f))))
	};

	std::vector<GameObject> scenePlaneGameObjects = {
		GameObject(1, -1, glm::scale(glm::translate(identity, glm::vec3(0.0f, -1.0f, 0.0f)), glm::vec3(1.f, 1.f, 1.f)))
	};
	// ---------------------------- Time stuff ---------------------------------
	Time::init();
	double timeAccumulator = 0.0;
	double initialTime = Time::now();
	double timeStepTaken = 0.05;
	double currentTime = initialTime;
	float viewportAspectRatio;
	// -------------------------------------------------------------------------

	// ---------------------------- Simple GUI ---------------------------------
	// Currently only displays fps using ImGui
	std::shared_ptr<GUI> gui = std::make_shared<GUI>();
	// -------------------------------------------------------------------------

	// ---------------------Example event broadcasting -------------------------
	Events::GameStart.broadcast();	// Invoke observers of the GameStart event
	
	// Broadcast "defaultSound" to observers of this event
	Events::TestAudioParameter.broadcast(defaultSound);
	// -------------------------------------------------------------------------

	//---Game Loop----
	while (!window.shouldClose()) {

		// Get current input events
		glfwPollEvents();

		// Update time-related values (fps, frame delta, etc.)
		Time::update();

		//Update the Time values for this frame
		timeAccumulator += Time::delta();
		currentTime = Time::now();

		//----Physics loop---(Unsure how physX works with this so it might change) -------//
		for (; timeAccumulator >= timeStepTaken; timeAccumulator -= timeStepTaken) {//Do per iteration
			scenePointLights[0]->setPos(2.f*glm::vec3(cosf(0.5f * ((float)(currentTime - initialTime))) , glm::abs(2.f * sinf(0.5f * ((float)(currentTime - initialTime)))) - 0.4f, 5.0f));
			sceneSpotLights[0]->setPos( glm::vec3(10.f * cosf(0.25f * ((float)(currentTime - initialTime))), 2.0f, 2.0f));
		}
		directionalLightCamera.setPos(glm::vec3(0.0f, 15.0f, -15.0f) + mainCamera.getPosition());
		//---Render Frame -----------------------------

		//Set the lighting properties of the scene
		renderPipeline->setDirectionalLightShadowMapProperties(directionalLightCamera.getView(), directionalLightCamera.getOrthographicProjection(), 4096, 4096);
		//Set render properties
		renderPipeline->setWindowDimentions(window.getWidth(), window.getHeight());

#if TRACKBALL_CAM
		if (window.getHeight() != 0) viewportAspectRatio = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
		else viewportAspectRatio = 0.0;
		renderPipeline->setCamera(mainCamera.getPosition(), mainCamera.getViewMatrix(), glm::perspective(glm::radians(60.f), viewportAspectRatio, 0.01f, 1000.0f) );
		audioSystem->setListenerPosition(mainCamera.getPosition());
		audioSystem->setListenerOrientation(mainCamera.getViewDirection(), mainCamera.getUpDirection());
#else
		mainRenderer.setCameraTransformations(mainCamera.getPosition(), mainCamera.getView(), mainCamera.getPerspectiveProjection);
#endif
		
		//Attach Objects to render
		for (auto object : sceneCubeGameObjects)	renderPipeline->attachRender(sceneRenderModels[0], object.getTransformation());
		for (auto object : scenePlaneGameObjects)	renderPipeline->attachRender(sceneRenderModels[1], object.getTransformation());
		
		//Render the output
		renderPipeline->executeRender();
		renderPipeline->clearRenderQueue();

		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui
		gui->draw();
		window.swapBuffers();

	}
	glfwTerminate();
	return 0;
}

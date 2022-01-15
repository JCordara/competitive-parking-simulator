#define GLEW_STATIC
#define AL_LIBTYPE_STATIC
#define FMT_HEADER_ONLY
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <limits>
#include <functional>
#include <math.h>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Camera.h"
#include "Lighting.h"
#include "GameObject.h"
#include "Model.h"
#include "GameCallback.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "AudioManager.h"

//Classes to test the manager with
class SizeTogglerTest{//This class is a resister for registering haha
	public: 
	SizeTogglerTest(std::shared_ptr<GameEventManager> em, Camera& c) : eventManager(em), mainCamera(c){
		function = bindMethodFunction_2_Variables(&Camera::windowSizeChanged, &mainCamera);
		enableWindowUpdate();
	};
	std::shared_ptr<GameEventManager> eventManager;
	Camera& mainCamera;
	unsigned int mainCameraid;
	GameEventManager::mousePositionFunction function;
	void enableWindowUpdate() {
		mainCameraid = eventManager->registerWindowSize(function);
	}
	void disableWindowUpdate() {
		eventManager->deregisterWindowSize(function, mainCameraid);
	}
};

class PlayCarSound {
public:
	PlayCarSound(AudioSource& c, Audio& s) : car(c), sound(s) {};
	AudioSource& car;
	Audio& sound;
	void playVROOOOOOM() {
		car.setPosition(glm::vec3(7.5f, 0.0f, 1.5f));
		car.playAudio(sound);
	}
};
//-------------------------------
int main() {

	Audio sound = AudioManager::instance().loadAudio("audio/rev.wav");
	AudioSource car = AudioManager::instance().createSource();
	car.setGain(1.5f);
	car.setPosition(glm::vec3(7.5f, 0.0f, 1.5f));
	car.setVelocity(glm::vec3(-10.0f, 0.0f, 0.0f));

	// ----------------------------

	Log::debug("Starting main");

	// WINDOW
	glfwInit();
	Window window(1200, 800, "Test Window");
	std::shared_ptr<GameEventManager> eventManager = std::make_shared<GameEventManager>();
	window.setCallbacks(eventManager);

	// -----Example--method--Registration--
	PlayCarSound testPlayer = PlayCarSound(car, sound);
	eventManager->registerKey(bindMethodFunction_0_Variables(&PlayCarSound::playVROOOOOOM, &testPlayer), GLFW_KEY_S, GLFW_PRESS, 0);
	// ------------------------------------
	GLDebug::enable();

	ShaderProgram shader("shaders/MainCamera.vert", "shaders/MainCamera.frag");
	glm::mat4 identiy(1.0f);

	//-----Cameras
	Camera mainCamera = Camera(glm::vec3(2.0f,2.5f,0.0f), glm::radians(45.0f), glm::radians(-45.0f), glm::radians(100.f), 3.f / 2.f, 0.01f, 1000.0f);
	SizeTogglerTest toggleSize = SizeTogglerTest(eventManager, mainCamera);
	eventManager->registerKey(bindMethodFunction_0_Variables(&SizeTogglerTest::enableWindowUpdate, &toggleSize), GLFW_KEY_T, GLFW_RELEASE, 0);
	eventManager->registerKey(bindMethodFunction_0_Variables(&SizeTogglerTest::disableWindowUpdate, &toggleSize), GLFW_KEY_T, GLFW_PRESS, 0);
	//-----Lights
	std::vector<PointLight> scenePointLights = {
		PointLight(glm::vec3(0.5f, 1.0f, 1.0f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 0.f, 0.f)),
		PointLight(glm::vec3(0.5f, 2.0f, -3.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(1.f, 0.f, 0.f))
	};
	glm::vec3 ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
	std::vector <std::vector<GLfloat>> lightRenderInfo;

	//-----Models
	std::vector<Model> sceneRenderModels = {
		Model(generateTestCubeModel(glm::vec3(0.0f, 0.2f, 1.0f)), glm::vec4(0.7f, 1.7f, 32.0f, 0.05f), true),
		Model(generateTestPlaneModel(glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec4(1.0f, 3.0f, 4.0f, 0.05f), true)
	};

	GPU_Geometry drawGeom;

	//------Objects
	std::vector<GameObject> sceneCubeGameObjects = {
		GameObject(0,-1, glm::translate(glm::scale(identiy,glm::vec3(1.f,1.f,1.f)), glm::vec3(0.0f, 0.0f, -2.0f))),
		GameObject(0,-1, glm::translate(glm::scale(identiy, glm::vec3(0.5f, 0.5f, 0.5f)), glm::vec3(0.0f, 0.0f, 0.0f)))
	};

	std::vector<GameObject> scenePlaneGameObjects = {
		GameObject(1, -1, glm::translate(glm::scale(identiy, glm::vec3(30.f, 1.f, 30.f)), glm::vec3(0.0f, -2.0f, 0.0f)))
	};

	car.playAudio(sound);
	//-------Real Time controls in (Seconds)
	double realTimePrevious = glfwGetTime(), realTimeCurrent = realTimePrevious, changeInTimeSince, timeAccumulator = 0.0f, initialTime = realTimePrevious;
	//---Game Loop----
	while (!window.shouldClose()) {
		glfwPollEvents();
		//Update the Time values for this frame
		realTimePrevious = realTimeCurrent;
		realTimeCurrent = glfwGetTime();
		changeInTimeSince = realTimeCurrent - realTimePrevious;
		timeAccumulator += changeInTimeSince;
		double timeStepTaken = 0.01f; //Temporary variable so it might be moved to an other spot
		//----Physics loop---(Unsure how physX works with this so it might change) -------//
		for (; timeAccumulator >= timeStepTaken; timeAccumulator -= timeStepTaken) {
			//Do per iteration
			//mainCamera.incrementTheta(glm::radians(0.3f));
			scenePointLights[0].setPos(glm::vec3(cosf(0.2f * ((float)(realTimeCurrent - initialTime))), sinf(0.2f * ((float)(realTimeCurrent - initialTime))), 1.0f));
		}

		//---Render Frame
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		shader.use();
		mainCamera.usePerspective(glGetUniformLocation(shader, "V"), glGetUniformLocation(shader, "P"), glGetUniformLocation(shader, "renderCameraPosition"));
		preparePointLightsForRendering(
			glGetUniformLocation(shader, "lightPositions"),
			glGetUniformLocation(shader, "lightColours"),
			glGetUniformLocation(shader, "lightAttenuationConstaints"),
			glGetUniformLocation(shader, "numberOfLights"),
			scenePointLights
		);
		glUniform3fv(glGetUniformLocation(shader, "ambientLight"), 1, &ambientLight[0]);

		prepareGameObjectsForRendering(glGetUniformLocation(shader, "Ms"), glGetUniformLocation(shader, "MsInverseTransposed"), sceneCubeGameObjects);
		sceneRenderModels[0].prepareModelForRendering(glGetUniformLocation(shader, "uniformPhongConstaints"), glGetUniformLocation(shader, "useColour"), drawGeom);
		sceneRenderModels[0].renderModel((int)sceneCubeGameObjects.size());
		prepareGameObjectsForRendering(glGetUniformLocation(shader, "Ms"), glGetUniformLocation(shader, "MsInverseTransposed"), scenePlaneGameObjects);
		sceneRenderModels[1].prepareModelForRendering(glGetUniformLocation(shader, "uniformPhongConstaints"), glGetUniformLocation(shader, "useColour"), drawGeom);
		sceneRenderModels[1].renderModel((int)scenePlaneGameObjects.size());

		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui
		window.swapBuffers();

		car.setPosition(car.position() + glm::vec3(-0.22f, 0.0f, 0.0f));
	}
	glfwTerminate();
	return 0;
}

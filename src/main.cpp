/** Some kind of big comment to document stuff probably goes here
* Authors, license, disclaimer, whatever
*/

// Build glew library statically
#define GLEW_STATIC

// Use header only FMT library to avoid unnecessary shared libraries
#define FMT_HEADER_ONLY

// For math functions
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <PhysX/PxPhysicsAPI.h>

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
#include "EditorCamera.h"
#include "Lighting.h"
#include "GameObject.h"
#include "Model.h"
#include "GameCallback.h"
#include "AudioManager.h"

// Include Time static class and initialize it's members
#include "Time.h"
double Time::lastFrameTime = glfwGetTime();
double Time::delta_ = 0.01; // Initialize to non-zero value

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TRACKBALL_CAM	1	// Use a trackball dev cam
#define PHYSX_TEST		0	// Run PhysX test function

/* This was generating a nasty warning
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
*/

void physX_test();

class PlayCarSound {
public:
	PlayCarSound(AudioSource& c, Audio& s) : car(c), sound(s) {};
	AudioSource& car;
	Audio& sound;
	void playVROOOOOOM() {
		car.playAudio(sound);
	}
};

//-------------------------------
int main() {
#if PHYSX_TEST
	physX_test();
#endif
	Audio& sound = AudioManager::instance().loadAudio("audio/rev.wav");
	AudioSource& car = AudioManager::instance().createStaticSource(glm::vec3(0.0f, 0.0f, -2.0f));

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
	//GLDebug::enable();

	ShaderProgram shader("shaders/MainCamera.vert", "shaders/MainCamera.frag");
	glm::mat4 identiy(1.0f);

	//-----Cameras

#if TRACKBALL_CAM
	EditorCamera mainCamera = EditorCamera();
	mainCamera.setPitch(-35.0);
	mainCamera.setYaw(-45.0);
	mainCamera.setZoom(5.5);
	mainCamera.setLookAt(glm::vec3(0.0f, 0.0f, -1.0f));

	// Register camera input events
	eventManager->registerMouseButton(bindMethodFunction_0_Variables(&EditorCamera::button1Down, &mainCamera), GLFW_MOUSE_BUTTON_1, GLFW_PRESS, 0);
	eventManager->registerMouseButton(bindMethodFunction_0_Variables(&EditorCamera::button1Up, &mainCamera), GLFW_MOUSE_BUTTON_1, GLFW_RELEASE, 0);
	eventManager->registerMouseButton(bindMethodFunction_0_Variables(&EditorCamera::button2Down, &mainCamera), GLFW_MOUSE_BUTTON_2, GLFW_PRESS, 0);
	eventManager->registerMouseButton(bindMethodFunction_0_Variables(&EditorCamera::button2Up, &mainCamera), GLFW_MOUSE_BUTTON_2, GLFW_RELEASE, 0);
	eventManager->registerMousePosition(bindMethodFunction_2_Variables(&EditorCamera::move, &mainCamera));
	eventManager->registerMousePosition(bindMethodFunction_2_Variables(&EditorCamera::rotateAroundTarget, &mainCamera));
	eventManager->registerScroll(bindMethodFunction_2_Variables(&EditorCamera::zoom, &mainCamera));
#else
	Camera mainCamera = Camera(glm::vec3(2.0f,2.5f,0.0f), glm::radians(45.0f), glm::radians(-45.0f), glm::radians(100.f), 3.f / 2.f, 0.01f, 1000.0f);
	SizeTogglerTest toggleSize = SizeTogglerTest(eventManager, mainCamera);
	eventManager->registerKey(bindMethodFunction_0_Variables(&SizeTogglerTest::enableWindowUpdate, &toggleSize), GLFW_KEY_T, GLFW_RELEASE, 0);
	eventManager->registerKey(bindMethodFunction_0_Variables(&SizeTogglerTest::disableWindowUpdate, &toggleSize), GLFW_KEY_T, GLFW_PRESS, 0);
#endif

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
		// Update time values for this frame using Time static class
		// Time::now(), Time::delta()
		Time::computeTimeDelta();

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

#if TRACKBALL_CAM
		float viewportAspectRatio = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
		glUniformMatrix4fv(glGetUniformLocation(shader, "P"), 1, GL_FALSE, glm::value_ptr(glm::perspective(glm::radians(60.f), viewportAspectRatio, 0.01f, 1000.0f)));
		glUniformMatrix4fv(glGetUniformLocation(shader, "V"), 1, GL_FALSE, glm::value_ptr(mainCamera.getViewMatrix()));
		glUniform3fv(glGetUniformLocation(shader, "renderCameraPosition"), 1, glm::value_ptr(mainCamera.getPosition()));

		AudioManager::instance().setListenerPosition(mainCamera.getPosition());
		AudioManager::instance().setListenerOrientation(mainCamera.getViewMatrix());
#else
		mainCamera.usePerspective(glGetUniformLocation(shader, "V"), glGetUniformLocation(shader, "P"), glGetUniformLocation(shader, "renderCameraPosition"));
#endif

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

		//car.setPosition(car.position() + glm::vec3(-0.22f, 0.0f, 0.0f));
	}
	glfwTerminate();
	return 0;
}



void physX_test() {

    // declare variables
    physx::PxDefaultAllocator      mDefaultAllocatorCallback;
    physx::PxDefaultErrorCallback  mDefaultErrorCallback;
    physx::PxDefaultCpuDispatcher* mDispatcher = NULL;
    physx::PxTolerancesScale       mToleranceScale;

    physx::PxFoundation* mFoundation = NULL;
    physx::PxPhysics* mPhysics = NULL;

    physx::PxScene* mScene = NULL;
    physx::PxMaterial* mMaterial = NULL;

    physx::PxPvd* mPvd = NULL;


    // init physx
    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
    if (!mFoundation) throw("PxCreateFoundation failed!");
    mPvd = PxCreatePvd(*mFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
    //mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(),true, mPvd);
    mToleranceScale.length = 100;        // typical length of an object
    mToleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
    mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);
    //mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale);

    physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = mDispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    mScene = mPhysics->createScene(sceneDesc);

    physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }


    // create simulation
    mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    physx::PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, physx::PxPlane(0, 1, 0, 50), *mMaterial);
    mScene->addActor(*groundPlane);

    float halfExtent = .5f;
    physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
    physx::PxU32 size = 30;
    physx::PxTransform t(physx::PxVec3(0));
    for (physx::PxU32 i = 0; i < size; i++) {
        for (physx::PxU32 j = 0; j < size - i; j++) {
            physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
            physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
            body->attachShape(*shape);
            physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            mScene->addActor(*body);
        }
    }
    shape->release();
    

    while (1)
    {
        mScene->simulate(1.0f / 60.0f);
        mScene->fetchResults(true);
    }

}

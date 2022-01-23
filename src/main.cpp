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
#include "Time.h"
#include "GUI.h"
#include "Renderers.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TRACKBALL_CAM	1	// Use a trackball dev cam
#define PHYSX_TEST		0	// Run PhysX test function

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
	Log::debug("Starting main");

#if PHYSX_TEST
	physX_test();
#endif

	// WINDOW
	glfwInit();
	Window window(1200, 800, "Test Window");
	std::shared_ptr<GameEventManager> eventManager = std::make_shared<GameEventManager>();
	window.setCallbacks(eventManager);

	// --------------------------- Example Audio -------------------------------
	Audio& sound = AudioManager::instance().loadAudio("audio/rev.wav");
	AudioSource& car = 
		AudioManager::instance().createStaticSource(glm::vec3(0.0f, 0.0f, 0.0f));
	// -------------------------------------------------------------------------

	// -----Example--method--Registration--
	PlayCarSound testPlayer = PlayCarSound(car, sound);
	eventManager->registerKey(bindMethodFunction_0_Variables(&PlayCarSound::playVROOOOOOM, &testPlayer), GLFW_KEY_S, GLFW_PRESS, 0);
	// ------------------------------------
	// screw gldebug
	//GLDebug::enable();

	//ShaderProgram shader("shaders/MainCamera.vert", "shaders/MainCamera.frag");
	MainRenderer mainRenderer;
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

	//-----Lights
	std::vector<PointLight> scenePointLights = {
		PointLight(glm::vec3(0.5f, 1.0f, 1.0f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 0.5f, 0.01f)),
		PointLight(glm::vec3(1.0f, 2.0f, -0.5f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.1f, 0.5f))
	};

	DirectionalLight dirLight = DirectionalLight(0.05f * glm::vec3(1.f, 1.f, 1.f), glm::normalize(glm::vec3(0.0f, -1.0f, 1.0f)));
	Camera directionalLightCamera = Camera(glm::vec3(0.0f, 15.0f, -15.0f), glm::radians(180.0f), glm::radians(-45.0f), 30.0f, 30.f, 0.f, 50.f, true);
	OrthographicDepthRenderer depthRenderer(2048, 2048);

	glm::vec3 ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);

	//-----Models
	std::vector<Model> sceneRenderModels = {
		Model(generateCubeGeometry(glm::vec3(1.0f, 1.0f, 1.0f)), glm::vec4(0.7f, 1.0f, 100.0f, 0.01f), true),
		Model(generatePlaneGeometry(glm::vec3(0.2f, 0.5f, 0.1f)), glm::vec4(1.0f, 1.0f, 50.0f, 0.01f), true),
		Model(generateSphereGeometry(glm::vec3(1.0f, 1.0f, 1.0f), 8, 8), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true)
	};

	//------Objects
	std::vector<GameObject> sceneCubeGameObjects = {
		GameObject(0,-1, glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f))),
		GameObject(0,-1, glm::scale(glm::translate(identity, glm::vec3(0.0f, 0.0f, 6.0f)), glm::vec3(1.0f, 2.0f, 0.5f)))
	};

	std::vector<GameObject> scenePlaneGameObjects = {
		GameObject(1, -1, glm::scale(glm::translate(identity, glm::vec3(0.0f, -1.0f, 0.0f)), glm::vec3(30.f, 1.f, 30.f)))
	};

	std::vector<GameObject> sceneSphereGameObjects = {
		GameObject(2, -1, glm::scale(glm::translate(identity, scenePointLights[0].getPos()), glm::vec3(0.1f, 0.1f, 0.1f))),
		GameObject(2, -1, glm::scale(glm::translate(identity, scenePointLights[1].getPos()), glm::vec3(0.1f, 0.1f, 0.1f)))
	};

	// ---------------------------- Time stuff ---------------------------------
	Time::init();
	double  timeAccumulator = 0.0, initialTime = Time::now(), timeStepTaken = 0.01, currentTime = initialTime;
	float viewportAspectRatio;
	// -------------------------------------------------------------------------

	// ---------------------------- Simple GUI ---------------------------------
	std::shared_ptr<GUI> gui = std::make_shared<GUI>();
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
			scenePointLights[0].setPos(2.f*glm::vec3(cosf(0.5f * ((float)(currentTime - initialTime))) , glm::abs(2.f * sinf(0.5f * ((float)(currentTime - initialTime)))) - 0.4f, 5.0f));
			sceneSphereGameObjects[0].setTransformation(glm::scale(glm::translate(identity, scenePointLights[0].getPos()), glm::vec3(0.1f, 0.1f, 0.1f)));
		}
		//----Render Directional Light DepthTexture -----
		depthRenderer.use(directionalLightCamera);
		depthRenderer.render(sceneCubeGameObjects, sceneRenderModels[0]);
		depthRenderer.render(scenePlaneGameObjects, sceneRenderModels[1]);
		depthRenderer.render(sceneSphereGameObjects, sceneRenderModels[2]);
		depthRenderer.endUse();
		//---Render Frame -----------------------------
		window.resetViewport();
		mainRenderer.use();
#if TRACKBALL_CAM
		if (window.getHeight() != 0) viewportAspectRatio = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
		else viewportAspectRatio = 0.0;
		mainRenderer.setCameraTransformations(mainCamera.getPosition(), mainCamera.getViewMatrix(), glm::perspective(glm::radians(60.f), viewportAspectRatio, 0.01f, 1000.0f));
#else
		mainRenderer.setCameraTransformations(mainCamera.getPosition(), mainCamera.getView(), mainCamera.getPerspectiveProjection);
#endif
		mainRenderer.setPointLights(scenePointLights);
		mainRenderer.setDirectionalLight(dirLight);
		mainRenderer.setAmbientLight(ambientLight);
		mainRenderer.useShadowMappingOnDirectionalLight(directionalLightCamera);
		depthRenderer.bindTextureForUse();
		mainRenderer.render(sceneCubeGameObjects, sceneRenderModels[0]);
		mainRenderer.render(scenePlaneGameObjects, sceneRenderModels[1]);
		mainRenderer.render(sceneSphereGameObjects, sceneRenderModels[2]);
		depthRenderer.unbindTextureForUse();
		mainRenderer.endUse();

		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui
		gui->draw();
		window.swapBuffers();
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

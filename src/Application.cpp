#include "Application.h"

Application::Application(appSettings& settings) 
	: settings(settings)
	, mainCamera()
	, directionalLightCamera(glm::vec3(0.0f, 15.0f, -15.0f), 
		glm::radians(180.0f), 
		glm::radians(-45.0f), 
		100.0f, 
		50.f, 
		5.f, 
		50.f, 
		true)
	, identity(1.0f)
{
	//App initialization
	glfwInit();

	/* Framework (Managers) - used by systems*/
	eventManager = std::make_shared<GameEventManager>();
	audioManager = std::make_shared<AudioManager>();
	scene  		 = std::make_shared<Scene>();
	window 		 = std::make_shared<Window>(eventManager, 1200, 800, "Test Window");

	/* Game systems - update() every frame */
	gameplay = std::make_shared<GameplaySystem>(scene, eventManager, audioManager);
	physics  = std::make_shared<PhysicsSystem>(scene, eventManager, audioManager);
	render   = std::make_shared<RenderSystem>(scene, eventManager, audioManager, window);

	/* Temporary stuff for now */
	// screw gldebug
	// GLDebug::enable();

	// ------------------------------ Cameras ----------------------------------
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

	// -------------------------------------------------------------------------
	renderPipeline = std::make_shared<GameRenderPipeline>();
	//-----Lights
	scenePointLights.push_back(
		std::make_shared<PointLight>(
			glm::vec3(0.5f, 1.0f, 1.0f), 
			glm::vec3(1.f, 1.f, 1.f), 
			glm::vec3(1.f, 0.5f, 0.01f))
	);
	scenePointLights.push_back(
		std::make_shared<PointLight>(
			glm::vec3(1.0f, 2.0f, -0.5f), 
			glm::vec3(1.f, 0.f, 0.f), 
			glm::vec3(1.f, 0.1f, 0.5f))
	);

	sceneSpotLights.push_back(
		std::make_shared<SpotLight>(
			glm::vec3(0.0f, 2.0f, 2.0f), 
			glm::vec3(1.f, 1.f, 1.f), 
			glm::vec3(1.f, 0.01f, 0.00f), 
			glm::normalize(glm::vec3(0.0f, -1.f, 0.0f)), 
			glm::radians(12.0f), 
			glm::radians(40.0f))
	);
	sceneSpotLights.push_back(
		std::make_shared<SpotLight>(
			glm::vec3(-1.5f, 1.0f, 0.0f), 
			glm::vec3(0.f, 0.f, 1.f), 
			glm::vec3(1.f, 0.01f, 0.00f), 
			glm::normalize(glm::vec3(-1.f, -1.f, 0.f)), 
			glm::radians(30.0f), 
			glm::radians(55.0f))
	);

	dirLight = std::make_shared<DirectionalLight>(
		0.3f * glm::vec3(1.f, 1.f, 1.f), 
		glm::normalize(glm::vec3(0.0f, -1.0f, 1.0f))
	);

	directionalLightCamera = Camera(
		glm::vec3(0.0f, 15.0f, -15.0f), 
		glm::radians(180.0f), 
		glm::radians(-45.0f), 
		100.0f, 
		50.f, 
		5.f, 
		50.f, 
		true
	);
	
	ambientLight = std::make_shared<AmbientLight>(
		0.05f * glm::vec3(1.0f, 1.0f, 1.0f));

	for (auto light : scenePointLights) renderPipeline->addPointLight(light);
	for (auto light : sceneSpotLights) renderPipeline->addSpotLight(light);
	renderPipeline->setDirectionalLight(dirLight);
	renderPipeline->setAmbientLight(ambientLight);

	//-----Models
	sceneRenderModels.push_back(
		std::make_shared<Model>("models/Test1.obj", glm::vec3(1.0, 1.0, 1.0))
	);
	sceneRenderModels.push_back(
		std::make_shared<Model>("models/Test2_M.obj", glm::vec3(1.0, 0.0, 1.0))
	);

	//------Objects
	sceneCubeGameObjects.push_back(
		GameObject(
			0,
			-1, 
			glm::translate(
				identity, 
				glm::vec3(0.0f, 0.0f, 0.0f)
			)
		)
	);
	sceneCubeGameObjects.push_back(
		GameObject(
			0,
			-1, 
			glm::scale(
				glm::translate(
					identity, 
					glm::vec3(0.0f, 0.0f, 6.0f)
				), 
				glm::vec3(1.0f, 2.0f, 0.5f)
			)
		)
	);
	sceneCubeGameObjects.push_back(
		GameObject(
			0,
			-1, 
			glm::scale(
				glm::rotate(
					glm::translate(
						identity, 
						glm::vec3(6.0f, 1.0f, 6.0f)
					),
					glm::radians(30.f),
					glm::vec3(0.f,1.f,0.f)
				), 
				glm::vec3(1.0f, 1.0f, 1.0f)
			)
		)
	);

	scenePlaneGameObjects.push_back(
		GameObject(
			1, 
			-1, 
			glm::scale(
				glm::translate(
					identity, 
					glm::vec3(0.0f, -1.0f, 0.0f)
				), 
				glm::vec3(1.f, 1.f, 1.f)
			)
		)
	);

	// ---------------------------- Time stuff ---------------------------------
	Time::init();
	timeAccumulator = 0.0;
	initialTime = Time::now();
	timeStepTaken = 0.05;
	currentTime = initialTime;

	// ---------------------------- Simple GUI ---------------------------------
	// Currently only displays fps using ImGui
	gui = std::make_shared<GUI>();
	// -------------------------------------------------------------------------

	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::PressW),
		GLFW_KEY_W, GLFW_PRESS, 0
	);

	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::PressS),
		GLFW_KEY_S, GLFW_PRESS, 0
	);

	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::PressA),
		GLFW_KEY_A, GLFW_PRESS, 0
	);

	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::PressD),
		GLFW_KEY_D, GLFW_PRESS, 0
	);

	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::ReleaseW),
		GLFW_KEY_W, GLFW_RELEASE, 0
	);

	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::ReleaseS),
		GLFW_KEY_S, GLFW_RELEASE, 0
	);

	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::ReleaseA),
		GLFW_KEY_A, GLFW_RELEASE, 0
	);

	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::ReleaseD),
		GLFW_KEY_D, GLFW_RELEASE, 0
	);
	
	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::PressSpace),
		GLFW_KEY_SPACE, GLFW_PRESS, 0
	);

	eventManager->registerKey(
		bindMethodFunction_0_Variables(&Event<void>::broadcast, &Events::ReleaseSpace),
		GLFW_KEY_SPACE, GLFW_RELEASE, 0
	);

	/* Bind functions here */
	Events::PressW.registerHandler<startAccelerateForwardsMode>();
	Events::ReleaseW.registerHandler<releaseDriveControls>();

	Events::PressS.registerHandler<startAccelerateReverseMode>();
	Events::ReleaseS.registerHandler<releaseDriveControls>();

	Events::PressA.registerHandler<startTurnHardLeftMode>();
	Events::ReleaseA.registerHandler<releaseLeftTurnControls>();

	Events::PressD.registerHandler<startTurnHardRightMode>();
	Events::ReleaseD.registerHandler<releaseRightTurnControls>();

	Events::PressSpace.registerHandler<startBrakeMode>();
	Events::ReleaseSpace.registerHandler<startBrakeMode>();

}

int Application::play() {
	// Invoke observers of the GameStart event
	Events::GameStart.broadcast();

	//Game loop
	while (!window->shouldClose()) {
		// Get current input events
		glfwPollEvents();

		// Update time-related values (fps, frame delta, etc.)
		Time::update();

		gameplay->update();
		physics->update();
		render->update();


		// And then a whole bunch of crap -----------------

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
		renderPipeline->setWindowDimentions(window->getWidth(), window->getHeight());

		if (window->getHeight() != 0) viewportAspectRatio = static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight());
		else viewportAspectRatio = 0.0;
		renderPipeline->setCamera(mainCamera.getPosition(), mainCamera.getViewMatrix(), glm::perspective(glm::radians(60.f), viewportAspectRatio, 0.01f, 1000.0f) );
		audioManager->setListenerPosition(mainCamera.getPosition());
		audioManager->setListenerOrientation(mainCamera.getViewDirection(), mainCamera.getUpDirection());

		glm::mat4 transformationPhysX;
		physics->PhysXMat4ToglmMat4(physx::PxMat44(gVehicle4W->getRigidDynamicActor()->getGlobalPose()),transformationPhysX);

		//Attach Objects to render
		for (auto object : sceneCubeGameObjects)	renderPipeline->attachRender(sceneRenderModels[0], transformationPhysX);
		for (auto object : scenePlaneGameObjects)	renderPipeline->attachRender(sceneRenderModels[1], object.getTransformation());
		
		//Render the output
		renderPipeline->executeRender();
		renderPipeline->clearRenderQueue();

		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui
		gui->draw();
		window->swapBuffers();
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

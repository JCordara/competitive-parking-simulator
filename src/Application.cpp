#include "Application.h"

int g_carsParked = 0;
void carParked() {
	g_carsParked++;
}
void carUnParked() {
	//g_carsParked--;
}

Application::Application(appSettings& settings) 
	: settings(settings)
	, mainCamera()
	, directionalLightCamera(glm::vec3(0.0f, 15.0f, -15.0f), 
		glm::radians(180.0f), 
		glm::radians(-45.0f), 
		100.0f, 50.f, 5.f, 50.f, true)
	, identity(1.0f)
{
	//App initialization
	glfwInit();
	Time::init();

	/* Framework (Managers) - used by systems*/
	scene        = Scene::newScene();
	window 		 = std::make_shared<Window>(1200, 800, "Test Window");
	inputManager = std::make_shared<InputManager>(window);
	audioSystem  = std::make_shared<AudioSystem>();

	/* Game systems - update() every frame */
	gameplay     = std::make_shared<GameplaySystem>(scene);
	physics      = std::make_shared<PhysicsSystem>(scene);
	render       = std::make_shared<RenderSystem>(scene, window);
	gui 	     = std::make_shared<GUI>();


	/* Temporary stuff for now */
	// screw gldebug
	// GLDebug::enable();

	// ------------------------------ Cameras ----------------------------------
	mainCamera.setPitch(-35.0);
	mainCamera.setYaw(-45.0);
	mainCamera.setZoom(5.5);
	mainCamera.setLookAt(glm::vec3(0.0f, 0.0f, -1.0f));

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
		std::make_shared<Model>("models/car1.obj", glm::vec3(1.0, 1.0, 1.0))
	);
	sceneRenderModels.push_back(
		std::make_shared<Model>("models/smileplane.obj", glm::vec3(1.0, 0.0, 1.0))
	);
	sceneRenderModels.push_back(
		std::make_shared<Model>("models/parkingstall.obj", glm::vec3(1.0, 0.0, 1.0))
	);

	//------Objects
	glm::vec3 box1Pos;
	physics->PhysXVec3ToglmVec3(box1->getGlobalPose().p,box1Pos);

	glm::vec3 box2Pos;
	physics->PhysXVec3ToglmVec3(box2->getGlobalPose().p, box2Pos);

	glm::vec3 box3Pos;
	physics->PhysXVec3ToglmVec3(box3->getGlobalPose().p, box3Pos);

	sceneCubeGameObjects.push_back(
		GameObject(
			0,
			-1, 
			glm::translate(
				identity,
				box1Pos
			)
		)
	);
	sceneCubeGameObjects.push_back(
		GameObject(
			0,
			-1, 
			glm::translate(
				identity, 
				box2Pos
			)
		)
	);
	sceneCubeGameObjects.push_back(
		GameObject(
			0,
			-1,
			glm::translate(
				identity,
				box3Pos
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
	scenePlaneGameObjects.push_back(
		GameObject(
			2, 
			-1, 
			glm::translate(
				identity, 
				glm::vec3(5.0f, -0.9f, 0.0f)
			)
		)
	);



	CarObjects.push_back(
		GameObject(
			0,
			-1,
			glm::translate(
				identity,
				glm::vec3(0.0f, 0.0f, 0.0f)
			)
		)
	);
	

	// --- Keyboard input binding ---
	// Create an axis between S and W for vehicle acceleration (keyboard)
	inputManager->createAxis(GLFW_KEY_W, GLFW_KEY_S, &Events::PlayerAccelerate);
	// Create an axis between A and D for vehicle steering (keyboard)
	inputManager->createAxis(GLFW_KEY_D, GLFW_KEY_A, &Events::PlayerSteer);
	// Bind the shift key to the handbrake event
	inputManager->bindInput(GLFW_KEY_LEFT_SHIFT, &Events::PlayerHandbrake);
	
	// --- Controller input binding ---
	// Create an axis from the left and right triggers (controller)
	inputManager->createAxis(
		GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 
		GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, 
		&Events::PlayerAccelerate, 
		ControlAxis::AXIS
	);
	// Bind the X (square on playstation) to the handbrake event
	inputManager->bindInput(GLFW_GAMEPAD_BUTTON_X, &Events::PlayerHandbrake);
	// Bind left joystick x-axis to steering
	inputManager->bindInput(GLFW_GAMEPAD_AXIS_LEFT_X, &Events::PlayerSteer);

	// --- Event binding ---
	// Vehicle behavior
	Events::PlayerAccelerate.registerHandler<vehicleAccelerateMode>();
	Events::PlayerSteer.registerHandler<vehicleTurnMode>();
	Events::PlayerHandbrake.registerHandler<vehicleHandbrakeMode>();

	// Camera behavior
	Events::CameraRotate.registerHandler<EditorCamera, &EditorCamera::rotateAroundTarget>(&mainCamera);
	Events::CameraZoom.registerHandler<EditorCamera, &EditorCamera::zoom>(&mainCamera);

	// Parking stall hack
	auto parkingStall = scene->addEntity();
	parkingStall->getComponent<TransformComponent>()->translate(5.0f, 0.0f, 0.0f);
	parkingStall->addComponent<VolumeTriggerComponent>();
	Events::CarParked.registerHandler<carParked>();
	Events::CarUnParked.registerHandler<carUnParked>();

}

int Application::play() {

	// Invoke observers of the GameStart event
	Events::GameStart.broadcast();

	//Game loop
	while (!window->shouldClose()) {
		
		// Process input
		inputManager->processInput();

		// Update time-related values
		Time::update();
	
		// Fixed time step game loop
		while (Time::takeNextStep()) {

			gameplay->update();
			physics->update();

			// Move lights
			float elapsedTime = (float)(Time::lastUpdateTime() - Time::programStartTime());

			scenePointLights[0]->setPos(
				2.f * glm::vec3(
					cosf(0.5f * elapsedTime), 
					glm::abs(2.f * sinf(0.5f * elapsedTime)) - 0.4f, 
					5.0f
				)
			);

			sceneSpotLights[0]->setPos(
				glm::vec3(
					10.f * cosf(0.25f * elapsedTime),
					2.0f, 
					2.0f
				)
			);
		}

		//---Render Frame -----------------------------
		// render->update();
		glm::mat4 transformationPhysX;
		physics->PhysXMat4ToglmMat4(physx::PxMat44(gVehicle4W->getRigidDynamicActor()->getGlobalPose()), transformationPhysX);

		directionalLightCamera.setPos(glm::vec3(0.0f, 15.0f, -15.0f) + glm::vec3(transformationPhysX[3]));
		//Set the lighting properties of the scene
		renderPipeline->setDirectionalLightShadowMapProperties(directionalLightCamera.getView(), directionalLightCamera.getOrthographicProjection(), 4096, 4096);
		mainCamera.setLookAt(glm::vec3(transformationPhysX[3]));
		//Set render properties
		renderPipeline->setWindowDimentions(window->getWidth(), window->getHeight());

		if (window->getHeight() != 0) viewportAspectRatio = static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight());
		else viewportAspectRatio = 0.0;
		renderPipeline->setCamera(mainCamera.getPosition(), mainCamera.getViewMatrix(), glm::perspective(glm::radians(60.f), viewportAspectRatio, 0.01f, 1000.0f) );
		audioSystem->setListenerPosition(mainCamera.getPosition());
		audioSystem->setListenerOrientation(mainCamera.getViewDirection(), mainCamera.getUpDirection());

		glm::mat4 box1PhysX;
		physics->PhysXMat4ToglmMat4(physx::PxMat44(gVehicle4W->getRigidDynamicActor()->getGlobalPose()),transformationPhysX);

		physics->PhysXMat4ToglmMat4(physx::PxMat44(box1->getGlobalPose()), box1PhysX);
		glm::mat4 box2PhysX;
		physics->PhysXMat4ToglmMat4(physx::PxMat44(box2->getGlobalPose()), box2PhysX);

		glm::mat4 box3PhysX;
		physics->PhysXMat4ToglmMat4(physx::PxMat44(box3->getGlobalPose()), box3PhysX);


		sceneCubeGameObjects[0].setTransformation(box1PhysX);
		sceneCubeGameObjects[1].setTransformation(box2PhysX * glm::scale(identity, glm::vec3(1.0f, 2.0f, 0.5f)));
		sceneCubeGameObjects[2].setTransformation(box3PhysX);

		
		//Attach Objects to render
		for (auto object : sceneCubeGameObjects)	renderPipeline->attachRender(sceneRenderModels[0], object.getTransformation());
		CarObjects[0].setTransformation(transformationPhysX /** glm::scale(identity, glm::vec3(1.25f, 1.0f, 2.5f))*/);
		renderPipeline->attachRender(sceneRenderModels[0], CarObjects[0].getTransformation() );
		
		renderPipeline->attachRender(sceneRenderModels[1], glm::scale(scenePlaneGameObjects[0].getTransformation(), glm::vec3(20,20,20)));
		renderPipeline->attachRender(sceneRenderModels[2], glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -0.9f, 0.0f)));
		
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

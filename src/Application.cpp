#include "Application.h"
#include <Random.h>

#define SPAWN_PROP_CARS 1
const unsigned int g_numAiCars = 4;

unsigned int playerId = 0;
std::vector<unsigned int> aiList;
std::unordered_map<unsigned int, int> scores;
std::shared_ptr<Entity> playerCar;

std::vector<glm::vec3> hedgeLocation;
std::vector<float> hedgeRotation;

std::vector<glm::vec3> curbLocation;
std::vector<float> curbRotation;

std::vector<glm::vec3> metalFenceLocation;
std::vector<float> metalFenceRotation;

std::vector<glm::vec3> warningWallLocation;
std::vector<float> warningWallRotation;

std::vector<glm::vec3> woodFenceLocation;
std::vector<float> woodFenceRotation;

std::vector<glm::vec3> woodFencePoleLocation;
std::vector<float> woodFencePoleRotation;

std::vector<glm::vec3> treeLocation;

std::vector<glm::vec3> parkingLineLocation;
std::vector<float> parkingLineRotation;

std::vector<glm::vec3> roadLocation;

vector<glm::vec3> collectGLMVecFromFile(string filepath, vector<glm::vec3> vec){
	std::ifstream file(filepath);
	std::string str;
	std::string space = " ";
	size_t pos = 0;

	float x, y, z;

	while(std::getline(file, str)) {
		int counter = 0;
		while((pos = str.find(space)) != string::npos) {
			counter++;
			if(counter == 1){
				x = std::stof(str.substr(0, pos));
			} else if(counter == 2){
				y = std::stof(str.substr(0, pos));
			}

			str.erase(0, pos + space.length());
		}

		z = std::stof(str);

		vec.push_back(glm::vec3(x,y,z));
	}

	file.close();

	return vec;
}

vector<float> collectfloatFromFile(string filepath, vector<float> vec){
	std::ifstream file(filepath);
	std::string str;

	while(std::getline(file, str)) {
		vec.push_back(std::stof(str));
	}

	file.close();

	return vec;
}

Application::Application(appSettings& settings): 
	settings(settings)
{
	//App initialization
	glfwInit();
	Time::init();
	Random::init();
	/* Framework - used by systems*/
	window   = std::make_shared<Window>(1200, 800, "Competitive Parking Simulator");
	scene    = std::make_shared<Scene>();
	guiScene = std::make_shared<GuiScene>(window);

	/* Game systems - update() every frame */
	input    = std::make_shared<InputSystem>(window);
	gameplay = std::make_shared<GameplaySystem>(scene);
	physics  = std::make_shared<PhysicsSystem>(scene);
	render   = std::make_shared<RenderSystem>(scene, guiScene, window);
	audio    = std::make_shared<AudioSystem>(scene);

	// Example gui creation
	// guiScene->addLabel(0.5f, 0.0f, "Test label em0", 0);
	// guiScene->addLabel(0.5f, 0.2f, "Test label em1", 1);
	// guiScene->addLabel(0.5f, 0.4f, "Test label em2", 2);
	// guiScene->addLabel(0.05f, 0.6f, "Test label em3", 3);
	// guiScene->addButton(0.1f, 0.7f, "Button em0", Events::CarUnParked, 0);
	// guiScene->addButton(0.4f, 0.7f, "Button em1", Events::GameStart, 1);
	// guiScene->addButton(0.7f, 0.7f, "Button em2", Events::GameStart, 2);
	// guiScene->addCheckbox(0.3f, 0.3f, "Test checkbox", Events::TestUiEvent);
	guiScene->addSlider(0.01f, 0.1f, "Music Volume", Events::ChangeMusicVolume, 0.1f);
	guiScene->addSlider(0.01f, 0.2f, "AI Opponents", Events::ChangeNumAI, 4, 0, 10);

	/* --------------------- Game World Description ------------------------ */

	// --- Entities ---
	playerCar = scene->addEntity();
	vector<sp<Entity>> aiCars;

	auto mapRoad  	 = scene->addEntity();
	auto mapMall     = scene->addEntity();
	auto mapMallText = scene->addEntity();
	auto mapGrass = scene->addEntity();
	auto mapWall1 = scene->addEntity();
	auto mapWall2 = scene->addEntity();
	auto mapWall3 = scene->addEntity();

	auto triggerBox = scene->addEntity();

	auto environmentalLight = scene->addEntity();
	auto mainCamera = playerCar->addChild();
	auto shadowCamera = playerCar->addChild();

	// --- Car Models ---
	auto modelPlayerCar = std::make_shared<Model>(
		"models/car1chassis.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelWheel = std::make_shared<Model>(
		"models/car1wheel.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	auto modelAiCar = std::make_shared<Model>(
		"models/car2chassis.obj", glm::vec3(.6f, .6f, .6f));

	auto modelPropCar1 = std::make_shared<Model>(
		"models/car3.obj", glm::vec3(.8f, .5f, .6f));

	auto modelPropCar2 = std::make_shared<Model>(
		"models/car4.obj", glm::vec3(.8f, .5f, .6f));

	auto modelPropCar3 = std::make_shared<Model>(
		"models/sedan.obj", glm::vec3(.8f, .5f, .6f));

	auto modelPropCar4 = std::make_shared<Model>(
		"models/truck.obj", glm::vec3(.8f, .5f, .6f));


	// --- Static Map Models ---
	auto modelMapRoad = std::make_shared<Model>(
		"models/cpsMap_PLotPlane.obj", glm::vec3(.0f, .0f, .0f));

	auto modelMapMall = std::make_shared<Model>(
		"models/cpsMap_Mall.obj", glm::vec3(.5f, .1f, .2f));
	
	auto modelMapMallText = std::make_shared<Model>(
		"models/cpsMap_MallText.obj", glm::vec3(.5f, .7f, .2f));

	auto modelMapWall1 = std::make_shared<Model>(
		"models/cpsMap_BWall1.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapWall2 = std::make_shared<Model>(
		"models/cpsMap_BWall2.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapWall3 = std::make_shared<Model>(
		"models/cpsMap_BWall3.obj", glm::vec3(.5f, .1f, .2f));

	auto modelMapGrass = std::make_shared<Model>(
		"models/cpsMap_BGGrass.obj", glm::vec3(.1f, .1f, .1f));


	// --- Instantiated Map Models ---
	auto modelMapMetalFence = std::make_shared<Model>(
		"models/cpsMap_MetalFence.obj", glm::vec3(.9f, .5f, .1f));

	auto modelMapCurb = std::make_shared<Model>(
		"models/cpsMap_Curb.obj", glm::vec3(.2f, .5f, .5f));

	auto modelMapHedge = std::make_shared<Model>(
		"models/cpsMap_Hedge.obj", glm::vec3(.6f, .8f, .5f));

	auto modelMapWoodFence = std::make_shared<Model>(
		"models/cpsMap_Woodfence.obj", glm::vec3(.3, .5, 1.0f));

	auto modelMapWoodFencePole = std::make_shared<Model>(
		"models/cpsMap_WoodfencePole.obj", glm::vec3(.3, .5, 1.0f));

	auto modelMapParkingLine = std::make_shared<Model>(
		"models/cpsMap_ParkingLine.obj", glm::vec3(.5f, .1f, .2f));
	
	auto modelMapTreeStump = std::make_shared<Model>(
		"models/cpsMap_TreeStump.obj", glm::vec3(.1f, .4f, .2f));

	auto modelMapTreeLeaves = std::make_shared<Model>(
		"models/cpsMap_TreeLeaves.obj", glm::vec3(.1f, .4f, .2f));

	auto modelMapWarningWall = std::make_shared<Model>(
		"models/cpsMap_WarningWall.obj", glm::vec3(.9f, .5f, .1f));
	
	auto modelMapBGRoad = std::make_shared<Model>(
		"models/cpsMap_BGRoad.obj", glm::vec3(.5f, .1f, .2f));


	// --- Directional light ---
	environmentalLight->addComponent<LightingComponent>(); 
	environmentalLight->getComponent<LightingComponent>()->setAmbient(glm::vec3(0.05f, 0.05f, 0.05f));
	environmentalLight->getComponent<LightingComponent>()->setDirectionalLight(glm::vec3(0.3f, 0.3f, 0.3f));

	// --- Main camera ---
	auto mainCameraTransform = mainCamera->getComponent<TransformComponent>();
	mainCameraTransform->setLocalPosition(0.0f, 12.0f, -2.0f);
	mainCameraTransform->setLocalRotation(glm::radians(-60.0f), glm::vec3(1.f, 0.f, 0.f));
	mainCameraTransform->localRotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate to face the other way
	
	auto mainCamerCam = mainCamera->addComponent<CameraComponent>();
	mainCamerCam->setPerspectiveCamera(glm::radians(100.f), 1.f /*Will be modified to the window*/,1.f, 130.f);

	auto MainDescription = mainCamera->addComponent<DescriptionComponent>();
	MainDescription->setInteger("Parent Global Y-Plane Forward Direction Projection", 1);

	// --- Shadow map camera ---
	auto shadowCameraTransform = shadowCamera->getComponent<TransformComponent>();
	auto q1 = physx::PxQuat(glm::radians(-45.f), physx::PxVec3(1.f, 0.f, 0.f));
	//auto q2 = physx::PxQuat(glm::radians(-90.f), physx::PxVec3(0.f, 1.f, 0.f));
	shadowCameraTransform->setLocalPosition(0, 100.0f, 100.0f);
	shadowCameraTransform->setLocalRotation( q1);

	auto shadowCameraCam = shadowCamera->addComponent<CameraComponent>();
	shadowCameraCam->setOrthographicCamera(200.f, 150.f, 10.f, 300.f);

	auto shadowCameraDesc = shadowCamera->addComponent<DescriptionComponent>();
	shadowCameraDesc->setInteger("Ignore parent rotations in render", 1);

	
	// --- Player car ---	
	// Add wheel entities
	for (int i = 0; i < 4; i++) {
		auto wheel = playerCar->addChild();
		
		auto mc = wheel->addComponent<ModelComponent>();
		mc->setModel(modelWheel);

		auto rc = wheel->addComponent<RendererComponent>();
		rc->enableRender();

		auto desc = wheel->addComponent<DescriptionComponent>();
		desc->setInteger("wheel", 1);
	}
	
	auto playerModel = playerCar->addComponent<ModelComponent>();
	playerModel->setModel(modelPlayerCar);
	
	auto playerRender = playerCar->addComponent<RendererComponent>();
	playerRender->enableRender();

	auto playerLight = playerCar->addComponent<LightingComponent>();
	playerLight->setSpotLight(glm::vec3(0.94, 0.89, 0.54), glm::vec3(1.0f, 0.f, 0.f), glm::radians(15.f), glm::radians(30.f));
	
	playerCar->addComponent<VehicleComponent>();
	
	auto playerController = playerCar->addComponent<ControllerComponent>();
	playerController->createAxis(GLFW_KEY_W, GLFW_KEY_S, &Events::VehicleAccelerate);
	playerController->createAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, &Events::VehicleAccelerate, ControlAxis::AXIS);
	playerController->createAxis(GLFW_KEY_D, GLFW_KEY_A, &Events::VehicleSteer);
	playerController->createAxis(GLFW_GAMEPAD_AXIS_LEFT_X, &Events::VehicleSteer);
	playerController->bindInput(GLFW_KEY_F, &Events::GameReset);
	playerController->bindInput(GLFW_KEY_LEFT_SHIFT, &Events::VehicleHandbrake);
	playerController->bindInput(GLFW_GAMEPAD_BUTTON_SQUARE, &Events::VehicleHandbrake);

	auto playerAudio = playerCar->addComponent<AudioComponent>();
	playerAudio->addEngineSound("audio/engine.wav", playerCar->getComponent<VehicleComponent>());

	auto playerDescription = playerCar->addComponent<DescriptionComponent>();
	playerDescription->setVec3("Forward", glm::vec3(0.f, 0.f, 1.f));


	// --- AI cars ---	
	for (int i = 0; i < g_numAiCars; i++) {
		auto aiCar = scene->addEntity();

		auto tc = aiCar->getComponent<TransformComponent>();
		tc->setLocalRotation(glm::radians(Random::randomFloat(0.0f, 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f));

		// Add wheel entities
		for (int i = 0; i < 4; i++) {
			auto wheel = aiCar->addChild();
			
			auto mc = wheel->addComponent<ModelComponent>();
			mc->setModel(modelWheel);

			auto rc = wheel->addComponent<RendererComponent>();
			rc->enableRender();

			auto desc = wheel->addComponent<DescriptionComponent>();
			desc->setInteger("wheel", 1);
		}
		
		auto mc = aiCar->addComponent<ModelComponent>();
		mc->setModel(modelAiCar);
		
		auto rc = aiCar->addComponent<RendererComponent>();
		rc->enableRender();
		
		aiCar->addComponent<VehicleComponent>();
		aiCar->addComponent<AiComponent>();
		auto audioComponent = aiCar->addComponent<AudioComponent>();
		audioComponent->addSound(AudioTrigger::Collision, "audio/oof.wav");
		aiCars.push_back(aiCar);
	}


	// --- Map road ---
	auto mapRoadTransform = mapRoad->getComponent<TransformComponent>();

	auto mapRoadModel = mapRoad->addComponent<ModelComponent>();
	mapRoadModel->setModel(modelMapRoad);

	auto mapRoadRender = mapRoad->addComponent<RendererComponent>();
	mapRoadRender->enableRender();

	// --- Map Background Grass ---
	auto mapGrassTransform = mapGrass->getComponent<TransformComponent>();

	auto mapGrassModel = mapGrass->addComponent<ModelComponent>();
	mapGrassModel->setModel(modelMapGrass);

	auto mapGrassRender = mapGrass->addComponent<RendererComponent>();
	mapGrassRender->enableRender();

	// --- Map Mall ---
	auto mapMallTransform = mapMall->getComponent<TransformComponent>();
	mapMallTransform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapMallModel = mapMall->addComponent<ModelComponent>();
	mapMallModel->setModel(modelMapMall);

	auto mapMallRender = mapMall->addComponent<RendererComponent>();
	mapMallRender->enableRender();

	auto mapMallRigidbody = mapMall->addComponent<RigidbodyComponent>();
	mapMallRigidbody->addActorStaticMesh(*modelMapMall, convert<physx::PxTransform>(mapMallTransform->getGlobalMatrix()));

	// --- Map Mall Text ---
	auto mapMallTextTransform = mapMallText->getComponent<TransformComponent>();
	mapMallTextTransform->localTranslate(0.0f, -5.5f, 0.0f);

	auto mapMallTextModel = mapMallText->addComponent<ModelComponent>();
	mapMallTextModel->setModel(modelMapMallText);

	auto mapMallTextRender = mapMallText->addComponent<RendererComponent>();
	mapMallTextRender->enableRender();

	// --- Map Boundary Walls ---
	// - Left Wall - 
	auto mapWall1Transform = mapWall1->getComponent<TransformComponent>();
	mapWall1Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall1Model = mapWall1->addComponent<ModelComponent>();
	mapWall1Model->setModel(modelMapWall1);

	auto mapWall1Rigidbody = mapWall1->addComponent<RigidbodyComponent>();
	mapWall1Rigidbody->addActorStaticMesh(*modelMapWall1, convert<physx::PxTransform>(mapWall1Transform->getGlobalMatrix()));

	// - Right Wall - 
	auto mapWall2Transform = mapWall2->getComponent<TransformComponent>();
	mapWall2Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall2Model = mapWall2->addComponent<ModelComponent>();
	mapWall2Model->setModel(modelMapWall2);

	auto mapWall2Rigidbody = mapWall2->addComponent<RigidbodyComponent>();
	mapWall2Rigidbody->addActorStaticMesh(*modelMapWall2, convert<physx::PxTransform>(mapWall2Transform->getGlobalMatrix()));
	
	// - Back Wall - 
	auto mapWall3Transform = mapWall3->getComponent<TransformComponent>();
	mapWall3Transform->localTranslate(0.0f, -1.0f, 0.0f);

	auto mapWall3Model = mapWall3->addComponent<ModelComponent>();
	mapWall3Model->setModel(modelMapWall3);

	auto mapWall3Rigidbody = mapWall3->addComponent<RigidbodyComponent>();
	mapWall3Rigidbody->addActorStaticMesh(*modelMapWall3, convert<physx::PxTransform>(mapWall3Transform->getGlobalMatrix()));

	// --- Map Metal Fence ---
	metalFenceLocation = collectGLMVecFromFile("../../res/modelTransformations/metalfenceLocation.txt", metalFenceLocation);
	metalFenceRotation = collectfloatFromFile("../../res/modelTransformations/metalfenceRotation.txt", metalFenceRotation);

	for(int i = 0; i < metalFenceLocation.size(); i++){
		auto mapMetalFence = scene->addEntity();
		auto mapMetalFenceTransform = mapMetalFence->getComponent<TransformComponent>();
		mapMetalFenceTransform->localTranslate(metalFenceLocation.at(i).x, metalFenceLocation.at(i).y, metalFenceLocation.at(i).z);
		mapMetalFenceTransform->localRotate(glm::radians(metalFenceRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapMetalFenceModel = mapMetalFence->addComponent<ModelComponent>();
		mapMetalFenceModel->setModel(modelMapMetalFence);

		auto mapMetalFenceRender = mapMetalFence->addComponent<RendererComponent>();
		mapMetalFenceRender->enableRender();
	}

	// --- Map Warning Wall ---
	warningWallLocation = collectGLMVecFromFile("../../res/modelTransformations/warningWallLocation.txt", warningWallLocation);
	warningWallRotation = collectfloatFromFile("../../res/modelTransformations/warningWallRotation.txt", warningWallRotation);

	for(int i = 0; i < warningWallLocation.size(); i++){
		auto mapwarningWall = scene->addEntity();
		auto mapwarningWallTransform = mapwarningWall->getComponent<TransformComponent>();
		mapwarningWallTransform->localTranslate(warningWallLocation.at(i).x, warningWallLocation.at(i).y, warningWallLocation.at(i).z);
		mapwarningWallTransform->localRotate(glm::radians(warningWallRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapwarningWallModel = mapwarningWall->addComponent<ModelComponent>();
		mapwarningWallModel->setModel(modelMapWarningWall);

		auto mapwarningWallRender = mapwarningWall->addComponent<RendererComponent>();
		mapwarningWallRender->enableRender();
	}
	
	// --- Map Curb ---
	curbLocation = collectGLMVecFromFile("../../res/modelTransformations/curbLocation.txt", curbLocation);
	curbRotation = collectfloatFromFile("../../res/modelTransformations/curbRotation.txt", curbRotation);

	for(int i = 0; i < curbLocation.size(); i++){
		auto mapCurb = scene->addEntity();
		auto mapCurbTransform = mapCurb->getComponent<TransformComponent>();
		mapCurbTransform->localTranslate(curbLocation.at(i).x, curbLocation.at(i).y, curbLocation.at(i).z);
		mapCurbTransform->localRotate(glm::radians(curbRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapCurbModel = mapCurb->addComponent<ModelComponent>();
		mapCurbModel->setModel(modelMapCurb);

		auto mapCurbRender = mapCurb->addComponent<RendererComponent>();
		mapCurbRender->enableRender();

		auto mapCurbRigidbody = mapCurb->addComponent<RigidbodyComponent>();
		mapCurbRigidbody->addActorStaticMesh(*modelMapCurb, convert<physx::PxTransform>(mapCurbTransform->getGlobalMatrix()));
	}

	// --- Map Background Road ---
	roadLocation = collectGLMVecFromFile("../../res/modelTransformations/roadLocation.txt", roadLocation);

	for(int i = 0; i < roadLocation.size(); i++){
		auto mapBGRoad = scene->addEntity();
		auto mapBGRoadTransform = mapBGRoad->getComponent<TransformComponent>();
		mapBGRoadTransform->localTranslate(roadLocation.at(i).x, roadLocation.at(i).y, roadLocation.at(i).z);

		auto mapBGRoadModel = mapBGRoad->addComponent<ModelComponent>();
		mapBGRoadModel->setModel(modelMapBGRoad);

		auto mapBGRoadRender = mapBGRoad->addComponent<RendererComponent>();
		mapBGRoadRender->enableRender();
	}

	// --- Map Hedges ---
	hedgeLocation = collectGLMVecFromFile("../../res/modelTransformations/hedgeLocation.txt", hedgeLocation);
	hedgeRotation = collectfloatFromFile("../../res/modelTransformations/hedgeRotation.txt", hedgeRotation);

	for(int i = 0; i < hedgeLocation.size(); i++){
		auto mapHedge = scene->addEntity();
		auto mapHedgeTransform = mapHedge->getComponent<TransformComponent>();
		mapHedgeTransform->localTranslate(hedgeLocation.at(i).x, hedgeLocation.at(i).y, hedgeLocation.at(i).z);
		mapHedgeTransform->localRotate(glm::radians(hedgeRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapHedgeModel = mapHedge->addComponent<ModelComponent>();
		mapHedgeModel->setModel(modelMapHedge);

		auto mapHedgeRender = mapHedge->addComponent<RendererComponent>();
		mapHedgeRender->enableRender();

		auto mapHedgeRigidbody = mapHedge->addComponent<RigidbodyComponent>();
		mapHedgeRigidbody->addActorStaticBox(physx::PxVec3(7.0f, 1.9f, 0.8f), convert<physx::PxTransform>(mapHedgeTransform->getGlobalMatrix()));
	}

	// --- Map Wood Fence ---
	woodFenceLocation = collectGLMVecFromFile("../../res/modelTransformations/woodfenceLocation.txt", woodFenceLocation);
	woodFenceRotation = collectfloatFromFile("../../res/modelTransformations/woodfenceRotation.txt", woodFenceRotation);

	for(int i = 0; i < woodFenceLocation.size(); i++){
		auto mapWoodFence = scene->addEntity();
		auto mapWoodFenceTransform = mapWoodFence->getComponent<TransformComponent>();
		mapWoodFenceTransform->localTranslate(woodFenceLocation.at(i).x, woodFenceLocation.at(i).y, woodFenceLocation.at(i).z);
		mapWoodFenceTransform->localRotate(glm::radians(woodFenceRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapWoodFenceModel = mapWoodFence->addComponent<ModelComponent>();
		mapWoodFenceModel->setModel(modelMapWoodFence);

		auto mapWoodFenceRender = mapWoodFence->addComponent<RendererComponent>();
		mapWoodFenceRender->enableRender();

		auto mapWoodFenceRigidbody = mapWoodFence->addComponent<RigidbodyComponent>();
		mapWoodFenceRigidbody->addActorStaticMesh(*modelMapWoodFence, convert<physx::PxTransform>(mapWoodFenceTransform->getGlobalMatrix()));
	}

	// --- Map Wood Fence Pole ---
	woodFencePoleLocation = collectGLMVecFromFile("../../res/modelTransformations/woodfencePoleLocation.txt", woodFencePoleLocation);
	woodFencePoleRotation = collectfloatFromFile("../../res/modelTransformations/woodfencePoleRotation.txt", woodFencePoleRotation);

	for(int i = 0; i < woodFencePoleLocation.size(); i++){
		auto mapWoodFencePole = scene->addEntity();
		auto mapWoodFencePoleTransform = mapWoodFencePole->getComponent<TransformComponent>();
		mapWoodFencePoleTransform->localTranslate(woodFencePoleLocation.at(i).x, woodFencePoleLocation.at(i).y, woodFencePoleLocation.at(i).z);
		mapWoodFencePoleTransform->localRotate(glm::radians(woodFencePoleRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapWoodFencePoleModel = mapWoodFencePole->addComponent<ModelComponent>();
		mapWoodFencePoleModel->setModel(modelMapWoodFencePole);

		auto mapWoodFencePoleRender = mapWoodFencePole->addComponent<RendererComponent>();
		mapWoodFencePoleRender->enableRender();

		auto mapWoodFencePoleRigidbody = mapWoodFencePole->addComponent<RigidbodyComponent>();
		mapWoodFencePoleRigidbody->addActorStaticMesh(*modelMapWoodFencePole, convert<physx::PxTransform>(mapWoodFencePoleTransform->getGlobalMatrix()));
	}

	// --- Map Trees ---
	treeLocation = collectGLMVecFromFile("../../res/modelTransformations/treeLocation.txt", treeLocation);

	for(int i = 0; i < treeLocation.size(); i++){
		// - Tree Stumps - 
		auto mapTreeStump = scene->addEntity();
		auto mapTreeStumpTransform = mapTreeStump->getComponent<TransformComponent>();
		mapTreeStumpTransform->localTranslate(treeLocation.at(i).x, treeLocation.at(i).y, treeLocation.at(i).z);

		auto mapTreeStumpModel = mapTreeStump->addComponent<ModelComponent>();
		mapTreeStumpModel->setModel(modelMapTreeStump);

		auto mapTreeStumpRender = mapTreeStump->addComponent<RendererComponent>();
		mapTreeStumpRender->enableRender();

		auto mapTreeStumpRigidbody = mapTreeStump->addComponent<RigidbodyComponent>();
		mapTreeStumpRigidbody->addActorStaticMesh(*modelMapTreeStump, convert<physx::PxTransform>(mapTreeStumpTransform->getGlobalMatrix()));

		// - Tree Leaves - 
		auto mapTreeLeaves = scene->addEntity();
		auto mapTreeLeavesTransform = mapTreeLeaves->getComponent<TransformComponent>();
		mapTreeLeavesTransform->localTranslate(treeLocation.at(i).x, treeLocation.at(i).y, treeLocation.at(i).z);
		mapTreeLeavesTransform->localRotate(Random::randomFloat(glm::radians(0.f), glm::radians(360.f)), glm::vec3(0.f, 1.f, 0.f));

		auto mapTreeLeavesModel = mapTreeLeaves->addComponent<ModelComponent>();
		mapTreeLeavesModel->setModel(modelMapTreeLeaves);

		auto mapTreeLeavesRender = mapTreeLeaves->addComponent<RendererComponent>();
		mapTreeLeavesRender->enableRender();
	}


	// --- Map Parking Lines ---
	parkingLineLocation = collectGLMVecFromFile("../../res/modelTransformations/parkingLinesLocation.txt", parkingLineLocation);
	parkingLineRotation = collectfloatFromFile("../../res/modelTransformations/parkingLinesRotation.txt", parkingLineRotation);

	for(int i = 0; i < parkingLineLocation.size(); i++){
		auto mapParkingLine = scene->addEntity();
		auto mapParkingLineTransform = mapParkingLine->getComponent<TransformComponent>();
		mapParkingLineTransform->localTranslate(parkingLineLocation.at(i).x, parkingLineLocation.at(i).y, parkingLineLocation.at(i).z);
		mapParkingLineTransform->localRotate(glm::radians(parkingLineRotation.at(i)), glm::vec3(0.f, 1.f, 0.f));

		auto mapParkingLineModel = mapParkingLine->addComponent<ModelComponent>();
		mapParkingLineModel->setModel(modelMapParkingLine);

		auto mapParkingLineRender = mapParkingLine->addComponent<RendererComponent>();
		mapParkingLineRender->enableRender();
	}

	
	/*
		Here is where we will collect Parking spot locations & rotations (Vector/Float to File Functions)
		Choose random spots for empty parking spots and put in separate vector (2 less than the AI + Player ( for(int i = 0; i < g_numAICars-1; i++) ))
		
		Instantiate prop cars in rest of parking spots that aren't gameplay spots (check empty parking spot vector through nested for loop)
			- Randomly choose between 4 different prop cars (car3, car4, sedan, truck)

		Create the trigger boxes and particles for empty parking spots here (for loop using size of empty parking spot location vector)
	*/

/*
#if SPAWN_PROP_CARS
	// --- Prop car ---
	for(int i = 0; i < sizeof(parkingVertices)/sizeof(*parkingVertices); i++){
		auto propCar = scene->addEntity();
		auto propCarTransform = propCar->getComponent<TransformComponent>();
		propCarTransform->setLocalPosition(parkingVertices[i]);
		
		if(i < 47){
			int randomRotate = rand() % 10;
			if(randomRotate <= 4) {
				propCarTransform->setLocalRotation(Random::randomFloat(glm::radians(80.f), glm::radians(100.f)), glm::vec3(0.f, 1.f, 0.f));	
			} else {
				propCarTransform->setLocalRotation(Random::randomFloat(glm::radians(260.f), glm::radians(280.f)), glm::vec3(0.f, 1.f, 0.f));
			}
		} else {
			int randomRotate = rand() % 10;
			if(randomRotate <= 4) {
				propCarTransform->setLocalRotation(Random::randomFloat(glm::radians(170.f), glm::radians(190.f)), glm::vec3(0.f, 1.f, 0.f));	
			} else {
				propCarTransform->setLocalRotation(Random::randomFloat(glm::radians(-10.f), glm::radians(10.f)), glm::vec3(0.f, 1.f, 0.f));	
			}
		}
		auto propCarModel = propCar->addComponent<ModelComponent>();
		propCarModel->setModel(modelPropCar);

		auto propCarRender = propCar->addComponent<RendererComponent>();
		propCarRender->enableRender();
		
		auto propCarRigidbody = propCar->addComponent<RigidbodyComponent>();
		propCarRigidbody->addActorDynamic(*modelPropCar, convert<physx::PxTransform>(propCarTransform->getGlobalMatrix()));
	}
#endif
	
	// --- Rocks ---
	sp<ModelComponent>     rockModel     = nullptr;
	sp<RendererComponent>  rockRender    = nullptr;
	sp<TransformComponent> rockTransform = nullptr;
	sp<RigidbodyComponent> rockRigidbody = nullptr;

	for (int i = 0; i < sizeof(rockPosVertices)/sizeof(*rockPosVertices); i++) {
		auto rock = scene->addEntity();
		rockTransform = rock->getComponent<TransformComponent>();
		rockModel = rock->addComponent<ModelComponent>();
		rockRender = rock->addComponent<RendererComponent>();
		rockRigidbody = rock->addComponent<RigidbodyComponent>();

		rockModel->setModel(modelRock);
		rockRender->enableRender();
		rockTransform->setLocalPosition(rockPosVertices[i]);
		rockTransform->setLocalRotation(Random::randomFloat(glm::radians(0.0f), glm::radians(360.0f)), glm::vec3(0.f, 1.f, 0.f));
	
		rockRigidbody->addActorStaticSphere(0.6f, convert<physx::PxTransform>(rockTransform->getGlobalMatrix()));
	}

	
	// --- TriggerBox ---
	auto triggerBoxComponent = triggerBox->addComponent<VolumeTriggerComponent>();
	for (int i = 0; i < sizeof(emptyparkingVertices) / sizeof(*emptyparkingVertices); i++) {
		if(emptyparkingVertices[i].x == -148.5f){
			triggerBoxComponent->createVolumeShape(PxTransform(PxVec3(emptyparkingVertices[i].x, 0.0f, emptyparkingVertices[i].z)), PxBoxGeometry(1.0f, 1.0f, 1.0f));
		}

		else {
			triggerBoxComponent->createVolumeShape(PxTransform(PxVec3(emptyparkingVertices[i].x, 0.0f, emptyparkingVertices[i].z)), PxBoxGeometry(1.0f, 1.0f, 1.0f));
		}
		
	}; 

*/

	/* --------------------- End Game World Description --------------------- */

	// Hacky stuff
	playerId = playerCar->id();
	scores[playerId] = 0;
	
	for (auto aiCar : aiCars) {
		gameplay->addAiId(aiCar->id());
		scores[aiCar->id()] = 0;
	}
	
	audio->setListener(mainCamera->getComponent<TransformComponent>());
}

int Application::play() {

	// Invoke observers of the GameStart event
	Events::GameStart.broadcast();

	//Game loop
	while (!window->shouldClose()) {

		// Process input
		input->processInput();

		// Update time-related values
		Time::update();
	
		// Fixed time step game loop
		while (Time::takeNextStep()) {
			if (scores[playerId] >= 5){//|| scores[aiList[0]] >= 5) {
			}
			else {
				gameplay->update();	// Gameplay / AI update
				physics->update();	// Physics update
				audio->update();	// Audio update
			}
		}

		// Render the current scene
		render->update();
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

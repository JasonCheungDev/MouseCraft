#include "RaceScene.h"

#include "OmegaEngineDefines.h"

#include <string>
#include <iostream>
#include <filesystem>
#include "Loading/PrefabLoader.h"
#include "Loading/ModelLoader.h"
#include "Core/ComponentFactory.h"
#include "Core/OmegaEngine.h"
#include "Rendering/Camera.h"
#include "Rendering/Lighting/DirectionalLight.h"
#include "Physics/PhysicsComponent.h"
#include "Common/FreeLookMovement.h"
#include "Common/PhysicsMover.h"
#include "Common/TransformAnimator.h"
#include "Car.h"
#include "Loading/ImageLoader.h"
#include "TriggerZone.h"
#include "CarEffects.h"
#include "CarTriggerSystem.h"
#include "Common/Rotator.h"
#include "UI/UIText.h"
#include "UI/UIRoot.h"
#include "UI/UIImage.h"
#include "PositionMatcher.h"
#include "OrientationMatcher.h"

namespace fs = std::experimental::filesystem;


RaceScene::RaceScene()
{
	// ===== LOAD PREFABS ===== //
	std::string path = "res/levels/race/load";
	for (const auto& entry : fs::directory_iterator(path))
		root.AddChild(PrefabLoader::LoadPrefab(entry.path().string()));

	// DEBUG
	auto e_light = EntityManager::Instance().Create();
	e_light->transform.rotate(glm::vec3(-0.35f, 0.35f, 0));
	auto c_light = ComponentFactory::Create<DirectionalLight>();
	e_light->AddComponent(c_light);
	root.AddChild(e_light);

	// ===== ADDITIONAL ENTITIES ===== //
	auto e_player = EntityManager::Instance().Create();
	// e_player->transform.setLocalPosition(glm::vec3(76, 0, -64));
	auto c_player = ComponentFactory::Create<Car>();
	c_player->acceleration = 1;
	c_player->steering = 0.006f;
	e_player->AddComponent(c_player);
	auto c_phys = ComponentFactory::Create<PhysicsComponent>(0.25f, 0.5f, 60, -64);
	c_phys->setAngularDrag(14);
	c_phys->setDrag(0.0f);	// car handles friction
	e_player->AddComponent(c_phys);
	e_player->name = "player";
	
	auto e_cam = EntityManager::Instance().Create();
	e_cam->transform.setLocalPosition(glm::vec3(0, 1.0f, 2.0f));
	// e_player->AddChild(e_cam);
	auto c_cam = ComponentFactory::Create<Camera>();
	c_cam->aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	c_cam->farPlane = 1000.0f;
	e_cam->AddComponent(c_cam);
	c_player->camera = c_cam;

	auto e_camHolder = EntityManager::Instance().Create();
	e_camHolder->AddChild(e_cam);
	root.AddChild(e_camHolder);
	auto c_camOrientation = ComponentFactory::Create<OrientationMatcher>();
	c_camOrientation->target = e_player;
	c_camOrientation->speed = 10.0f;
	e_camHolder->AddComponent(c_camOrientation);
	auto c_camPosition = ComponentFactory::Create<PositionMatcher>();
	c_camPosition->target = e_player;
	c_camPosition->speed = 25.0f;
	e_camHolder->AddComponent(c_camPosition);


	auto e_carModel = ModelLoader::Load("res/external/nahchimento/lambo.fbx", false);
	// e_model->transform.setLocalPosition(glm::vec3(0, 0.5f, 0));
	e_carModel->transform.setLocalScale(glm::vec3(0.0025f, 0.0025f, 0.0025f));
	e_carModel->transform.setLocalRotation(glm::vec3(0.0f, glm::radians(180.0f), 0.0f));
	e_player->AddChild(e_carModel);

	auto e_carWheelFL = EntityManager::Instance().Find("Lamborghini_Aventador:Wheel_FL", e_carModel);
	auto e_carWheelFR = EntityManager::Instance().Find("Lamborghini_Aventador:Wheel_FR", e_carModel);
	auto e_carWheelRL = EntityManager::Instance().Find("Lamborghini_Aventador:Wheel_RL", e_carModel);
	auto e_carWheelRR = EntityManager::Instance().Find("Lamborghini_Aventador:Wheel_RR", e_carModel);
	
	auto c_rotatorWheelFL = ComponentFactory::Create<Rotator>();
	c_rotatorWheelFL->rotationSpeed = glm::vec3(3.14f, 0, 0);
	e_carWheelFL->AddComponent(c_rotatorWheelFL);

	auto c_rotatorWheelFR = ComponentFactory::Create<Rotator>();
	c_rotatorWheelFR->rotationSpeed = glm::vec3(3.14f, 0, 0);
	e_carWheelFR->AddComponent(c_rotatorWheelFR);

	auto c_rotatorWheelRL = ComponentFactory::Create<Rotator>();
	c_rotatorWheelRL->rotationSpeed = glm::vec3(3.14f, 0, 0);
	e_carWheelRL->AddComponent(c_rotatorWheelRL);

	auto c_rotatorWheelRR = ComponentFactory::Create<Rotator>();
	c_rotatorWheelRR->rotationSpeed = glm::vec3(3.14f, 0, 0);
	e_carWheelRR->AddComponent(c_rotatorWheelRR);

	root.AddChild(e_player);

	c_player->wheelFL = c_rotatorWheelFL;
	c_player->wheelFR = c_rotatorWheelFR;
	c_player->wheelBL = c_rotatorWheelRL;
	c_player->wheelBR = c_rotatorWheelRR;


	// ====== UI ===== //
	auto e_uiRoot = EntityManager::Instance().Create();
	root.AddChild(e_uiRoot);
	auto c_uiRoot = ComponentFactory::Create<UIRoot>();
	e_uiRoot->AddComponent(c_uiRoot);

	auto e_speedDisplay = EntityManager::Instance().Create();
	e_uiRoot->AddChild(e_speedDisplay);
	auto c_speedText = ComponentFactory::Create<UIText>("SPEED: 0", 0, 0);
	c_speedText->vAnchor = VerticalAnchor::ANCHOR_BOTTOM;
	c_speedText->hAnchor = HorizontalAnchor::ANCHOR_LEFT;
	e_speedDisplay->AddComponent(c_speedText);
	
	//auto e_boostDisplay = EntityManager::Instance().Create();
	//e_uiRoot->AddChild(e_boostDisplay);
	//auto c_boostText = ComponentFactory::Create<UIImage>("res/textures/white.png", 0.8f, 20.0f, 0, 10.0f);
	//c_boostText->vAnchor = VerticalAnchor::ANCHOR_BOTTOM;
	//c_boostText->hAnchor = HorizontalAnchor::ANCHOR_LEFT;
	//e_boostDisplay->AddComponent(c_boostText);

	c_player->speedDisplay = EntityManager::Instance().Find("speed_display")->GetComponent<UIText>();
	c_player->boostDisplay = EntityManager::Instance().Find("boost_display")->GetComponent<UIImage>();

	// ===== ZONE: OPEN AIRS ===== //
	auto e_openAirStart = EntityManager::Instance().Create();
	e_openAirStart->transform.setLocalPosition(76, 0, -64);
	root.AddChild(e_openAirStart);
	auto c_openAirStartTrigger = ComponentFactory::Create<TriggerZone>();
	e_openAirStart->AddComponent(c_openAirStartTrigger);
	
	auto e_openAirExit = EntityManager::Instance().Create();
	e_openAirExit->transform.setLocalPosition(140, 0, -28);
	root.AddChild(e_openAirExit);
	auto c_openAirExitTrigger = ComponentFactory::Create<TriggerZone>();
	e_openAirExit->AddComponent(c_openAirExitTrigger);

	auto oaEffect = new SpeedModifierEffect(c_player, 0.8f);
	c_openAirStartTrigger->OnTriggerEnter.Attach(oaEffect->ActivateHandler);
	c_openAirExitTrigger->OnTriggerExit.Attach(oaEffect->DeactivateHandler);



	// ===== ZONE: GRAND GATES ===== //
	// generate rings 
	const auto startPos = glm::vec3(116, 1, -12);
	const auto posStep = glm::vec3(0,0,4);
	const auto startRot = glm::radians(20.0f);
	const auto rotStep = glm::radians(15.0f);
	const auto startDelay = 0.1f;
	const auto delayStep = 0.1f;
	const auto duration = 0.5f;
	const auto numRings = 8;
	const auto curve = new SineConverter();

	std::vector<TransformAnimator*> animators;
	std::vector<PointLight*> lights;
	for (int i = 0; i < numRings; i++)
	{
		auto e_halfRing = PrefabLoader::LoadPrefab("res/prefabs/race/ring_half.json");
		e_halfRing->transform.setLocalPosition(startPos + (float)i * posStep);
		e_halfRing->transform.setLocalScale(glm::vec3(0.33f));
		auto animIdle = new Animation();
		animIdle->duration = 5.0f;
		animIdle->AddRotation(0.0f, glm::vec3());
		auto animLeft = new Animation();
		animLeft->SetRotationPolicy(RotationPolicy::EULER);
		animLeft->SetCurve(curve);
		animLeft->duration = 5.0f;
		animLeft->AddRotation(0.0f, glm::vec3());
		animLeft->AddRotation(startDelay + i * delayStep, glm::vec3());
		animLeft->AddRotation(startDelay + i * delayStep + duration, glm::vec3(0,0, startRot + i * rotStep));
		auto animRight = new Animation();
		animRight->SetRotationPolicy(RotationPolicy::EULER);
		animLeft->SetCurve(curve);
		animRight->duration = 5.0f;
		animRight->AddRotation(0.0f, glm::vec3());
		animRight->AddRotation(startDelay + i * delayStep, glm::vec3());
		animRight->AddRotation(startDelay + i * delayStep + duration, glm::vec3(0, 0, -(startRot + i * rotStep)));
		auto c_tanim = ComponentFactory::Create<TransformAnimator>();
		c_tanim->SetOneShot(true);
		c_tanim->AddAnimation("idle", animIdle);
		c_tanim->AddAnimation("right", animRight);
		c_tanim->AddAnimation("left", animLeft);
		e_halfRing->AddComponent(c_tanim);
		//auto c_rotator = ComponentFactory::Create<Rotator>();
		//c_rotator->rotationSpeed = glm::vec3(0, 0, 3.14f);
		//e_halfRing->AddComponent(c_rotator);
		// retrieval all the good stuff 
		e_halfRing->GetAllComponents<TransformAnimator>(animators);
		e_halfRing->GetAllComponents<PointLight>(lights);
		// finalize
		root.AddChild(e_halfRing);
	}

	auto e_ggStart = EntityManager::Instance().Create();
	e_ggStart->transform.setLocalPosition(116, 0, -16);
	root.AddChild(e_ggStart);
	auto c_ggStart = ComponentFactory::Create<TriggerZone>();
	e_ggStart->AddComponent(c_ggStart);

	auto e_ggLeft = EntityManager::Instance().Create();
	e_ggLeft->transform.setLocalPosition(120, 0, 24);
	root.AddChild(e_ggLeft);
	auto c_ggLeft = ComponentFactory::Create<TriggerZone>();
	c_ggLeft->radius = 2.0f;
	e_ggLeft->AddComponent(c_ggLeft);

	auto e_ggRight = EntityManager::Instance().Create();
	e_ggRight->transform.setLocalPosition(108, 0, 24);
	root.AddChild(e_ggRight);
	auto c_ggRight = ComponentFactory::Create<TriggerZone>();
	c_ggRight->radius = 2.0f;
	e_ggRight->AddComponent(c_ggRight);

	auto e_ggExit = EntityManager::Instance().Create();
	e_ggExit->transform.setLocalPosition(116, 0, 40);
	root.AddChild(e_ggExit);
	auto c_ggExit = ComponentFactory::Create<TriggerZone>();
	e_ggExit->AddComponent(c_ggExit);

	// Bind all the components to together
	auto grandGatesSequence = new GrandGatesSequence(c_player, animators, lights, c_ggStart, c_ggLeft, c_ggRight, c_ggExit);
	// And that's it! the systems should activate the sequence properly


	// ===== ADDITIONAL SYSTEMS ===== //
	auto cts = new CarTriggerSystem();
	cts->SetCar(c_player);
	OmegaEngine::Instance().AddSystem(cts);
}


// ComponentFactory::Create<FreeLookMovement>();
	//c_player->moveSpeed = 20.0f;

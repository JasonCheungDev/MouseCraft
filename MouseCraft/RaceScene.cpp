#include "RaceScene.h"

#include "OmegaEngineDefines.h"

#include <string>
#include <iostream>
#include <filesystem>
#include "Loading/PrefabLoader.h"
#include "Loading/ModelLoader.h"
#include "Core/ComponentFactory.h"
#include "Rendering/Camera.h"
#include "Rendering/Lighting/DirectionalLight.h"
#include "Physics/PhysicsComponent.h"
#include "Common/FreeLookMovement.h"
#include "Common/PhysicsMover.h"
#include "Common/TransformAnimator.h"
#include "Car.h"
#include "Loading/ImageLoader.h"

namespace fs = std::experimental::filesystem;


RaceScene::RaceScene()
{
	auto e_player = EntityManager::Instance().Create();
	auto c_player = ComponentFactory::Create<Car>();
	c_player->speed = 5.0f;
	c_player->steering = 0.5f;
	e_player->AddComponent(c_player);
	auto c_phys = ComponentFactory::Create<PhysicsComponent>(0.25f, 0.5f);
	e_player->AddComponent(c_phys);

	auto e_cam = EntityManager::Instance().Create();
	e_cam->transform.setLocalPosition(glm::vec3(0, 1.0f, 2.0f));
	e_player->AddChild(e_cam);
	auto c_cam = ComponentFactory::Create<Camera>();
	c_cam->aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	c_cam->farPlane = 1000.0f;
	e_cam->AddComponent(c_cam);

	auto e_model = ModelLoader::Load("res/models/primitive/cube.obj");
	// e_model->transform.setLocalPosition(glm::vec3(0, 0.5f, 0));
	e_model->transform.setLocalScale(glm::vec3(0.25f, 0.25f, 0.5f));
	e_player->AddChild(e_model);

	// root.AddChild(PrefabLoader::LoadPrefab("res/external/karboosx/Legos/building_corner.json"));

	// root.AddChild(ModelLoader::Load("res/models/primitive/octagon.obj"));

	root.AddChild(e_player);

	std::string path = "res/levels/race/load";
	for (const auto& entry : fs::directory_iterator(path))
		root.AddChild(PrefabLoader::LoadPrefab(entry.path().string()));
}


// ComponentFactory::Create<FreeLookMovement>();
	//c_player->moveSpeed = 20.0f;

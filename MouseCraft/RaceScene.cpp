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
#include "Car.h"

namespace fs = std::experimental::filesystem;


RaceScene::RaceScene()
{
	auto e_cam = EntityManager::Instance().Create();
	root.AddChild(e_cam);
	auto c_cam = ComponentFactory::Create<Camera>();
	c_cam->aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	c_cam->farPlane = 1000.0f;
	e_cam->AddComponent(c_cam);
	auto c_player = 
		ComponentFactory::Create<FreeLookMovement>();
	c_player->moveSpeed = 20.0f;
	//	ComponentFactory::Create<Car>();
	//c_player->speed = 10.0f;
	e_cam->AddComponent(c_player);
	
	//auto c_phys = ComponentFactory::Create<PhysicsComponent>(1.0f, 1.0f);
	//e_cam->AddComponent(c_phys);

	e_cam->t().setLocalPosition(glm::vec3(0, 5.0f, 0));

	root.AddChild(PrefabLoader::LoadPrefab("res/external/karboosx/Legos/building_corner.json"));

	root.AddChild(ModelLoader::Load("res/models/primitive/octagon.obj"));

	std::string path = "res/levels/race/load";
	for (const auto& entry : fs::directory_iterator(path))
		root.AddChild(PrefabLoader::LoadPrefab(entry.path().string()));
}

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

	auto e_light = EntityManager::Instance().Create();
	e_light->transform.rotate(glm::vec3(0.24f, 0.24f, 0));
	auto c_light = ComponentFactory::Create<DirectionalLight>();
	e_light->AddComponent(c_light);

	root.AddChild(e_light);

	e_cam->t().setLocalPosition(glm::vec3(0, 5.0f, 0));

	auto e_dudes = EntityManager::Instance().Create();
	auto e_dude1 = ModelLoader::Load("res/models/nanosuit/nanosuit.obj");
	auto e_dude2 = ModelLoader::Load("res/models/nanosuit/nanosuit.obj");
	e_dude2->transform.translate(glm::vec3(5, 0, 0));
	e_dudes->AddChild(e_dude1);
	e_dudes->AddChild(e_dude2);

	auto rendercomponents = std::vector<RenderComponent*>();
	e_dudes->GetAllComponents(rendercomponents);

	root.AddChild(e_dudes);

	std::string path = "res/levels/race";
	for (const auto& entry : fs::directory_iterator(path))
		root.AddChild(PrefabLoader::LoadPrefab(entry.path().string()));
}

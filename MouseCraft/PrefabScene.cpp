#include "PrefabScene.h"

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
namespace fs = std::experimental::filesystem;

PrefabScene::PrefabScene()
{
	auto e_cam = EntityManager::Instance().Create();
	root.AddChild(e_cam);
	auto c_cam = ComponentFactory::Create<Camera>();
	c_cam->aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	c_cam->farPlane = 100.0f;
	e_cam->AddComponent(c_cam);

	auto c_player = 
		ComponentFactory::Create<PhysicsMover>();
		//ComponentFactory::Create<FreeLookMovement>();
	c_player->speed = 10.0f;
	e_cam->AddComponent(c_player);
	auto c_phys = ComponentFactory::Create<PhysicsComponent>(1.0f, 1.0f);
	e_cam->AddComponent(c_phys);

	std::string path = "res/levels/demo";
	for (const auto& entry : fs::directory_iterator(path))
		root.AddChild(PrefabLoader::LoadPrefab(entry.path().string()));
}

void PrefabScene::InitScene()
{

}

void PrefabScene::Update(const float delta)
{

}

void PrefabScene::CleanUp()
{

}
#include "LoadingScene.h"

#include "Core/OmegaEngine.h"
#include "Core/EntityManager.h"
#include "Core/ComponentFactory.h"
#include "Rendering/Camera.h"
#include "Loading/PrefabLoader.h"

LoadingScene::LoadingScene(Scene* toLoad) : nextScene(toLoad)
{
	auto e = EntityManager::Instance().Create();
	auto cam = ComponentFactory::Create<Camera>();
	e->AddComponent(cam);
	root.AddChild(e);
	root.AddChild(PrefabLoader::LoadPrefab("res/levels/loading/ui.json"));
}

LoadingScene::~LoadingScene()
{
}

void LoadingScene::InitScene()
{
}

void LoadingScene::Update(const float delta)
{
	// wait a few frames to display properly (kinda jank)
	--wait;
	if (wait < 0)
	{
		OmegaEngine::Instance().ChangeScene(nextScene);
	}
}

void LoadingScene::CleanUp()
{
}

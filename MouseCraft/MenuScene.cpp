#include "MenuScene.h"

#include "Core/OmegaEngine.h"
#include "Core/Scene.h"
#include "Core/EntityManager.h"
#include "Core/ComponentFactory.h"
#include "Rendering/Camera.h"
#include "Loading/PrefabLoader.h"
#include "Core/OmegaEngine.h"
#include "RaceScene.h"
#include "UINavigationSystem.h"
#include "LoadingScene.h"

MenuScene::MenuScene()
{
	auto e_cam = EntityManager::Instance().Create();
	auto c_cam = ComponentFactory::Create<Camera>();
	e_cam->AddComponent(c_cam);
	root.AddChild(e_cam);

	root.AddChild(PrefabLoader::LoadPrefab("res/levels/menu/ui.json"));

	instructionsPanel = EntityManager::Instance().Find("instructions");
	creditsPanel = EntityManager::Instance().Find("credits");

	auto playButton = EntityManager::Instance().Find("play_button")->GetComponent<UISelectable>();
	playButton->OnActivated.Attach(onPlayButtonActivated);

	auto instructionsButton = EntityManager::Instance().Find("instructions_button")->GetComponent<UISelectable>();
	instructionsButton->OnActivated.Attach(onInstructionsButtonActivated);

	auto creditsButton = EntityManager::Instance().Find("credits_button")->GetComponent<UISelectable>();
	creditsButton->OnActivated.Attach(onCreditsButtonActivated);

	auto iBackBtn = EntityManager::Instance().Find("i_back_button")->GetComponent<UISelectable>();
	iBackBtn->OnActivated.Attach(onBackButtonActivated);

	auto cBackBtn = EntityManager::Instance().Find("c_back_button")->GetComponent<UISelectable>();
	cBackBtn->OnActivated.Attach(onBackButtonActivated);
}

MenuScene::~MenuScene()
{
}

void MenuScene::InitScene()
{
}

void MenuScene::Update(const float delta)
{
}

void MenuScene::CleanUp()
{
}

void MenuScene::ShowInstructionsPanel(std::string data)
{
	instructionsPanel->SetEnabled(true);
	creditsPanel->SetEnabled(false);
	OmegaEngine::Instance().GetSystem<UINavigationSystem>()->Select("i_back_button");
}

void MenuScene::ShowCreditsPanel(std::string data)
{
	creditsPanel->SetEnabled(true);
	instructionsPanel->SetEnabled(false);
	OmegaEngine::Instance().GetSystem<UINavigationSystem>()->Select("c_back_button");
}

void MenuScene::HideAllPanels(std::string data)
{
	instructionsPanel->SetEnabled(false);
	creditsPanel->SetEnabled(false);
	OmegaEngine::Instance().GetSystem<UINavigationSystem>()->Select("play_button");
}

void MenuScene::StartGame(std::string data)
{
	OmegaEngine::Instance().ChangeScene(new LoadingScene(new RaceScene()));
}

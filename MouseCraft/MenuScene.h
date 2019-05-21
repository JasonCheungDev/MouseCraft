#pragma once

#include "Core/Scene.h"
#include "Event/Handler.h"

class MenuScene : public Scene
{
public:
	MenuScene();

	~MenuScene();

	void InitScene();

	void Update(const float delta);

	void CleanUp();

private:
	void ShowInstructionsPanel(std::string);

	void ShowCreditsPanel(std::string);

	void HideAllPanels(std::string);

	void StartGame(std::string);

	Handler<MenuScene, std::string> onInstructionsButtonActivated = Handler<MenuScene, std::string>(this, &MenuScene::ShowInstructionsPanel);
	Handler<MenuScene, std::string> onCreditsButtonActivated = Handler<MenuScene, std::string>(this, &MenuScene::ShowCreditsPanel);
	Handler<MenuScene, std::string> onBackButtonActivated = Handler<MenuScene, std::string>(this, &MenuScene::HideAllPanels);
	Handler<MenuScene, std::string> onPlayButtonActivated = Handler<MenuScene, std::string>(this, &MenuScene::StartGame);

	Entity* instructionsPanel;
	Entity* creditsPanel;
};


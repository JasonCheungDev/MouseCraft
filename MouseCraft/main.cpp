#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#include <Windows.h>
#include <iostream>
#include "Core/OmegaEngine.h"
#include "Core/UpdateSystem.h"
#include "Input/InputSystem.h"
#include "Loading/PrefabLoader.h"
#include "Network/NetworkSystem.h"
#include "Physics/PhysicsManager.h"
#include "Sound/SoundManager.h"
#include "UI/UIManager.h"
#include "Rendering/RenderingSystem.h"
#include "MainScene.h"
#include "PrefabScene.h"
#include "RaceScene.h"
#include "MenuScene.h"
#include "Rendering/TextRenderer.h"
#include "UINavigationSystem.h"

SoundManager* noise;

extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void SetupSound()
{
	//adding sound system
	noise = new SoundManager();
	//start initial music track, standard form for music selection
    selectSong(MenuBGM);
}

void MainTest()
{
	PrefabLoader::DumpLoaders();

	OmegaEngine::Instance().initialize();

	RenderingSystem* rs = new RenderingSystem();
	rs->SetSize(OmegaEngine::Instance().getWindow()->getWidth(), OmegaEngine::Instance().getWindow()->getHeight());

	// Add the systems
	OmegaEngine::Instance().AddSystem(rs);
	OmegaEngine::Instance().AddSystem(new UpdateSystem());
	OmegaEngine::Instance().AddSystem(&PhysicsManager::Instance());
	OmegaEngine::Instance().AddSystem(new InputSystem());
	OmegaEngine::Instance().AddSystem(NetworkSystem::Instance());
	OmegaEngine::Instance().AddSystem(new UINavigationSystem());
	OmegaEngine::Instance().AddSystem(new UIManager());

	// Extra loading here 
	TextRenderer::Instance().LoadFont("res/fonts/kenvector_future.ttf");
	TextRenderer::Instance().LoadFont("res/fonts/futur.ttf");
	TextRenderer::Instance().LoadFont("res/fonts/Quicksand-Regular.ttf");
	TextRenderer::Instance().LoadFont("res/fonts/Abel-Regular.ttf");
	TextRenderer::Instance().SetDefaultFont("res/fonts/Quicksand-Regular.ttf");

	OmegaEngine::Instance().ChangeScene(new MenuScene());

	//Start the game
	OmegaEngine::Instance().Loop();
}

int main(int argc, char* argv[])
{
	// SetupSound();

	MainTest();
}
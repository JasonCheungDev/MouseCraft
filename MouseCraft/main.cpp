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
//#include "UI/UIManager.h"
#include "Rendering/RenderingSystem.h"
#include "MainScene.h"
#include "PrefabScene.h"

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


	InputSystem* inputSystem = new InputSystem();

	RenderingSystem* rs = new RenderingSystem();
	rs->SetSize(OmegaEngine::Instance().getWindow()->getWidth(), OmegaEngine::Instance().getWindow()->getHeight());

	//Add the systems
	OmegaEngine::Instance().AddSystem(new UpdateSystem());
	OmegaEngine::Instance().AddSystem(&PhysicsManager::Instance());
	OmegaEngine::Instance().AddSystem(rs);
	OmegaEngine::Instance().AddSystem(inputSystem);
	OmegaEngine::Instance().AddSystem(NetworkSystem::Instance());
	//OmegaEngine::Instance().AddSystem(new UIManager());

	OmegaEngine::Instance().ChangeScene(new PrefabScene());

	//Start the game
	OmegaEngine::Instance().Loop();
}

int main(int argc, char* argv[])
{
	// SetupSound();

	MainTest();
}
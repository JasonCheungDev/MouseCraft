#pragma once

#include "Core/Scene.h"

// Very simple scene to display a static loading screen before target scene.
class LoadingScene : public Scene
{
public:
	LoadingScene(Scene* toLoad);
	~LoadingScene();

	void InitScene();

	void Update(const float delta);

	void CleanUp();

private:
	Scene* nextScene;
	int wait = 3;
};


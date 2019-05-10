#pragma once

#include "Core/Scene.h"

// A scene completely loaded through prefab json files.
class PrefabScene : public Scene
{
public:
	PrefabScene();

	void InitScene();

	void Update(const float delta);

	void CleanUp();
};


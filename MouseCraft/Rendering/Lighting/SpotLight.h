#pragma once

#include "Light.h"
#include <json.hpp>
#include "../../Loading/PrefabLoader.h"
using json = nlohmann::json;

class SpotLight : public Light
{
public:
	SpotLight();
	~SpotLight();
	float angle = 45.0f;

public:
	virtual void LoadUniforms(Shader* shader) {};
	virtual void PrepareShadowmap(Shader* shader) {};
	virtual void CleanupShadowmap(Shader* shader) {};

private:
	/* TEMPLATE
	{
		"type": "SpotLight",
		"color": [1.0, 1.0, 1.0],
		"intensity": 1.0,
		"range": 0.0,
		"angle": 45.0
	}
	*/
	static Component* Create(json json);
	static ComponentRegistrar reg;
};


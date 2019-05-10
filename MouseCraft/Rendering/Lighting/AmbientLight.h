#pragma once

#include "Light.h"
#include <json.hpp>
#include "../../Loading/PrefabLoader.h"
using json = nlohmann::json;

class AmbientLight : public Light
{
public:
	AmbientLight();
	~AmbientLight();
	
public:
	virtual void LoadUniforms(Shader* shader) {};
	virtual void PrepareShadowmap(Shader* shader) {};
	virtual void CleanupShadowmap(Shader* shader) {};

private:
	/* TEMPLATE
	{
		"type": "AmbientLight",
		"color": [1.0, 1.0, 1.0],
		"intensity": 1.0,
	}
	*/
	static Component* Create(json json);
	static ComponentRegistrar reg;
};


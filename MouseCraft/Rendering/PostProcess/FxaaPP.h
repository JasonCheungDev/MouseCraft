#pragma once

#include "PostProcess.h"
#include "../Shader.h"
#include "../Material.h"

class FxaaPP : public PostProcess
{
public:
	FxaaPP() : PostProcess()
	{
		_shader = std::make_unique<Shader>("res/shaders/pp_base_vertex.glsl", "res/shaders/pp_fxaa_fragment.glsl");
		_settings = std::make_unique<Material>();
	}
	~FxaaPP() {};
};


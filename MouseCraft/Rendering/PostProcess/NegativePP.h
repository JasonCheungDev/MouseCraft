#pragma once

#include "PostProcess.h"
#include "../Shader.h"
#include "../Material.h"

class NegativePP : public PostProcess
{
public:
	NegativePP() : PostProcess()
	{
		shader = std::make_unique<Shader>("res/shaders/pp_base_vertex.glsl", "res/shaders/pp_negative_fragment.glsl");
		settings = std::make_unique<Material>();
	}
	~NegativePP() {};
};


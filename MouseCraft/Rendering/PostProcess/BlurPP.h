#pragma once

#include "PostProcess.h"
#include "../Shader.h"
#include "../Material.h"

class BlurPP : public PostProcess
{
public:
	BlurPP() : PostProcess()
	{
		shader = std::make_unique<Shader>("res/shaders/pp_base_vertex.glsl", "res/shaders/pp_blur_fragment.glsl");
		settings = std::make_unique<Material>();
		settings->SetBool("u_Horizontal", true);
	}
	~BlurPP() {};
};


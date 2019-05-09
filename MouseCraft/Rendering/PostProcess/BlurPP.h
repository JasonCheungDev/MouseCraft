#pragma once

#include "PostProcess.h"
#include "../Shader.h"
#include "../Material.h"

class BlurPP : public PostProcess
{
public:
	BlurPP() : PostProcess()
	{
		_shader = std::make_unique<Shader>("res/shaders/pp_base_vertex.glsl", "res/shaders/pp_blur_fragment.glsl");
		_settings = std::make_unique<Material>();
		_settings->SetBool("u_Horizontal", true);
	}
	~BlurPP() {};
};


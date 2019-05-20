#pragma once

#include "PostProcess.h"
#include "../Shader.h"
#include "../Material.h"
#include "../Texture1x1.h"

class BlurPP : public PostProcess
{
public:
	BlurPP() 
	{
		_horBlur = new Shader("res/shaders/PostProcess/pp.vs", "res/shaders/PostProcess/pp_hor_blur_stencil.fs");
		_verBlur = new Shader("res/shaders/PostProcess/pp.vs", "res/shaders/PostProcess/pp_ver_blur_stencil.fs");
		_settings = std::make_unique<Material>();
		_settings->SetFloat("u_Strength", 1.0f);
		_settings->AddTexture("u_StencilTex", new Texture1x1(glm::vec4(1, 1, 1, 1)));
	}
	~BlurPP() {};

	Shader* GetActiveShader() override
	{
		return (_firstPass) ? _horBlur : _verBlur;
	}

	virtual bool Pass(unsigned int freeTexSlot)
	{
		GetSettings()->LoadMaterial(GetActiveShader(), freeTexSlot);
		_firstPass = !_firstPass;
		return _firstPass;
	}

private:
	bool _firstPass = true;
	Shader* _horBlur;
	Shader* _verBlur;
};


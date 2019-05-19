#pragma once

#include "PostProcess.h"
#include "../Shader.h"
#include "../Material.h"

class BlurPP : public PostProcess
{
public:
	BlurPP() 
	{
		_horBlur = new Shader("res/shaders/PostProcess/pp.vs", "res/shaders/PostProcess/pp_hor_blur.fs");
		_verBlur = new Shader("res/shaders/PostProcess/pp.vs", "res/shaders/PostProcess/pp_ver_blur.fs");
		_settings = std::make_unique<Material>();
		_settings->SetFloat("u_Strength", 1.0f);
	}
	~BlurPP() {};

	Shader* GetActiveShader() override
	{
		return (_firstPass) ? _horBlur : _verBlur;
	}

	virtual bool Pass(unsigned int freeTexSlot)
	{
		_settings->LoadMaterial(GetActiveShader());
		_firstPass = !_firstPass;
		return _firstPass;
	}

private:
	bool _firstPass = true;
	Shader* _horBlur;
	Shader* _verBlur;
};


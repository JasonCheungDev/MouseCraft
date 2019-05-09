#pragma once

#include "PostProcess.h"
#include "../FrameBuffer.h"

class BloomPP : public PostProcess
{
public:
	BloomPP()
	{
		_highLuminosity = new Shader("res/shaders/PostProcess/pp.vs", "res/shaders/PostProcess/pp_high_luminosity.fs");
		_horBlur        = new Shader("res/shaders/PostProcess/pp.vs", "res/shaders/PostProcess/pp_hor_blur.fs");
		_verBlur		= new Shader("res/shaders/PostProcess/pp.vs", "res/shaders/PostProcess/pp_ver_blur.fs");
		_bloom			= new Shader("res/shaders/PostProcess/pp.vs", "res/shaders/PostProcess/pp_bloom.fs");
		_fbo1			= new FrameBuffer();
		_fbo2			= new FrameBuffer();
		_settings		= std::make_unique<Material>();
		_settings->SetFloat("u_Strength", 2.0f);
	}

	Shader* GetActiveShader() override
	{
		switch (_currentPass)
		{
		case BloomPP::LUMINOSITY:
			return _highLuminosity;
			break;
		case BloomPP::HOR_BLUR:
			return _horBlur;
			break;
		case BloomPP::VER_BLUR:
			return _verBlur;
		case BloomPP::BLOOM:
			return _bloom;
			break;
		default:
			break;
		}
	}
	
	virtual bool RequestingGBuffer()
	{
		return false;
	}

	virtual bool RequestingBackBuffer()
	{
		return _currentPass == LUMINOSITY || _currentPass == BLOOM;
	}

	virtual bool DrawingToBackBuffer()
	{
		return _currentPass == BLOOM;
	}

	virtual bool Pass(unsigned int freeTexSlot)
	{
		// load settings 
		_settings->LoadMaterial(GetActiveShader());

		// does NOT set order (be careful with the sampler ordering in the shader!)
		if (_currentPass == PASS::LUMINOSITY)
		{
			// back-buffer is Read()
			_fbo1->Draw();
			_currentPass = PASS::HOR_BLUR;
		}
		else if (_currentPass == PASS::HOR_BLUR)
		{
			_fbo1->Read(freeTexSlot);
			_fbo2->Draw();
			_currentPass = PASS::VER_BLUR;

		}
		else if (_currentPass == PASS::VER_BLUR)
		{
			_fbo2->Read(freeTexSlot);
			_fbo1->Draw();
			_currentPass = PASS::BLOOM;

		}
		else if (_currentPass == PASS::BLOOM)
		{
			_bloom->setInt("u_BlmTex", freeTexSlot);
			_fbo1->Read(freeTexSlot);
			// back-buffer is Draw()
			_currentPass = PASS::LUMINOSITY;
		}

		// notify if finished 
		return _currentPass == LUMINOSITY;
	}

private:
	enum PASS
	{
		LUMINOSITY,	// 1st pass
		HOR_BLUR,	// 2nd pass
		VER_BLUR,	// 3rd pass 
		BLOOM,		// 4th pass (final)
	};
	PASS _currentPass;
	Shader* _highLuminosity;
	Shader* _horBlur; 
	Shader* _verBlur;
	Shader* _bloom;
	FrameBuffer* _fbo1;
	FrameBuffer* _fbo2;
};


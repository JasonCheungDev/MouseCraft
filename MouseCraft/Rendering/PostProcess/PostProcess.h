#pragma once

#include "../Shader.h"
#include "../Material.h"
#include <memory>

class PostProcess
{
public:
	bool enabled = true;

	virtual Shader* GetActiveShader()
	{
		return _shader.get();
	}

	void SetShader(std::unique_ptr<Shader> shader)
	{
		_shader = std::move(shader);
	}

	Material* GetMaterial()
	{
		return _settings.get();
	}

	void SetMaterial(std::unique_ptr<Material> settings)
	{
		_settings = std::move(settings);
	}

	// if true then the g-buffer textures will be loaded into GetShader(). 
	virtual bool RequestingGBuffer()
	{
		return true;
	}

	// if true then the back-buffer texture will be loaded into GetShader().
	virtual bool RequestingBackBuffer()
	{
		return true;
	}

	// if true then the back-buffer will be set as render target
	// else set your own render target.
	virtual bool DrawingToBackBuffer()
	{
		return true;
	}

	// do final prep before entire screen is rendered into render target
	virtual bool Pass(unsigned int freeTexSlot)
	{
		_settings->LoadMaterial(_shader.get(), freeTexSlot);
		return true;
	}

protected:
	std::unique_ptr<Shader> _shader;
	std::unique_ptr<Material> _settings;
};


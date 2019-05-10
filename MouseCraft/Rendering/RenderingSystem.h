#pragma once

#include <vector>
#include "../Util/OpenGLProfiler.h"
#include "../Util/CpuProfiler.h"
#include "Renderable.h"
#include "RenderComponent.h"
#include "Camera.h"
#include "Lighting\Light.h"
#include "Lighting\DirectionalLight.h"
#include "Lighting\PointLight.h"
#include "../Core/System.h"
#include "PostProcess\PostProcess.h"
#include "FrameBuffer.h"
#include "GBuffer.h"
#include "CubeMap.h"

class RenderingSystem : public System
{
public:
	Camera* activeCamera; 
	Shader* geometryShader;		// default geometry shader 
	Shader* compDLightShader;	// default composition shader for directional lights
	Shader* compPLightShader;	// default composition shader for point lights
	Shader* shadowmapShader;	// default shadowmap shader 
	Shader* imageShader;		// default UI shader 
	Shader* postShader;			// default postprocessing shader 
	Shader* postToScreenShader;	// default shader to move final texture to back buffer
	Shader* skyboxShader;		// default shader for skybox

private:
	OpenGLProfiler profiler;
	CpuProfiler cpuProfiler;
	unsigned int quadVAO, cubeVAO;
	GBuffer* gbuffer;
	FrameBuffer* ppWriteFBO;
	FrameBuffer* ppReadFBO;
	unsigned int screenWidth, screenHeight; 
	CubeMap* skyboxTexture;

	std::map<std::string, std::unique_ptr<PostProcess>> _postProcesses;

	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 uiProjection;

public:
	RenderingSystem();
	~RenderingSystem();
	void SetSize(unsigned int width, unsigned int height);
	void SetCamera(Camera* camera);
	virtual void Update(float dt) override;
	// virtual void addComponent(std::type_index t, Component* component) override;
	// virtual void clearComponents() override;
	void LoadFont(std::string path);
	void addPostProcess(const std::string name, std::unique_ptr<PostProcess> postProcess);
	void removePostProcess(const std::string name);
	PostProcess* getPostProcess(const std::string name);
	void setSkybox(CubeMap* cubemap);

private: 
	
	void RenderGeometryPass();

	void RenderDirectionalLightingPass();	// composition
		
	void RenderPointLightingPass();			// composition 

	void RenderShadowMapsPass();

	void RenderSkyboxPass();

	void RenderPostProcessPass();

	void RenderUIImagesPass();

	void RenderUITextPass();

	void RenderEntityGeometry(Entity* e, glm::mat4 transform);
	void RenderCompositionPass();
	void InitializeFrameBuffers();
	void InitializeScreenQuad();
	void InitializeScreenCube();
	void DrawComponent(RenderComponent* component);
};


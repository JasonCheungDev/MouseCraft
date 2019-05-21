#include "RenderingSystem.h"

#include <typeinfo>
#include <typeindex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "Constants.h"
#include "Shader.h"
#include "Texture.h"
#include "Texture1x1.h"
#include "Lighting/Light.h"
#include "Lighting/AmbientLight.h"
#include "Lighting/DirectionalLight.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"
#include "PostProcess/PostProcess.h"
#include "PostProcess/NegativePP.h"
#include "PostProcess/BlurPP.h"
#include "PostProcess/BloomPP.h"
#include "TextRenderer.h"
#include "../UI/UIImage.h"
#include "../UI/UIText.h"
#include "../Loading/TextureLoader.h"
#include "../Core/ComponentManager.h"
#include "../Core/OmegaEngine.h"
#include "../Core/ComponentList.h"


void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}




RenderingSystem::RenderingSystem() : System()
{
	//// During init, enable debug output
	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(MessageCallback, 0);


	// system initialization 
	// onlyReceiveFrameUpdates = true;
	
	// create profiler 
	profiler.InitializeTimers(10);		// 1 for each pass so far 
	cpuProfiler.InitializeTimers(10);	// 1 for each pass, 1 for general use, 1 for initialization
	cpuProfiler.LogOutput("RenderingSystem.log")
		.FormatMilliseconds(true);
		//.PrintOutput(true);
	
	cpuProfiler.StartTimer(7);

	// initialize default shaders  
	geometryShader = new Shader("res/shaders/geometry.vs", "res/shaders/geometry.fs");
	Material* geometryDefaults		= new Material();
	Texture1x1* diffuseDefaultTex	= new Texture1x1(glm::vec3(1.0f));
	geometryDefaults->AddTexture(SHADER_TEX_DIFFUSE, diffuseDefaultTex);
	Texture1x1* normalDefaultTex	= new Texture1x1(glm::vec3(0.0f, 0.0f, 1.0f));
	geometryDefaults->AddTexture(SHADER_TEX_NORMAL, normalDefaultTex);
	geometryShader->defaultSettings = geometryDefaults;

	ambientLightShader		= new Shader("res/shaders/screen.vs", "res/shaders/Light/ambient.fs");
	directionalLightShader	= new Shader("res/shaders/screen.vs", "res/shaders/Light/directional.fs");
	pointLightShader		= new Shader("res/shaders/screen.vs", "res/shaders/Light/point.fs");
	spotLightShader			= new Shader("res/shaders/screen.vs", "res/shaders/Light/spot.fs");
	shadowmapShader = new Shader("res/shaders/shadowmap_vertex.glsl", "res/shaders/shadowmap_fragment.glsl");
	imageShader = new Shader("res/shaders/image_vertex.glsl", "res/shaders/image_fragment.glsl");
	postShader = new Shader("res/shaders/post_vertex.glsl", "res/shaders/post_fragment.glsl");
	postToScreenShader = new Shader("res/shaders/post2screen_vertex.glsl", "res/shaders/post2screen_fragment.glsl");
	skyboxShader = new Shader("res/shaders/skybox.vs", "res/shaders/skybox.fs");


	// initialize frame buffers for geometry rendering pass 
	// InitializeFrameBuffers(); waiting for screen size
	
	// create a quad that covers the screen for the composition pass 
	InitializeScreenQuad();
	// create a cube that can be used to render cubemaps
	InitializeScreenCube();

	// built-in post processing 
	// pseudo FXAA
	auto fxaa = std::make_unique<PostProcess>();
	fxaa->enabled = false;
	fxaa->SetShader(std::make_unique<Shader>("res/shaders/PostProcess/pp.vs", "res/shaders/PostProcess/pp_fxaa.fs"));
	addPostProcess("FXAA", std::move(fxaa));
	// fog 
	auto fogPp = std::make_unique<PostProcess>();	// too lazy to make a dedicated class
	fogPp->SetShader(std::make_unique<Shader>("res/shaders/pp_base_vertex.glsl", "res/shaders/pp_fog_fragment.glsl"));
	fogPp->SetSettings(std::make_unique<Material>());	// u_FogDensity, u_FogStart, u_FogColor
	fogPp->enabled = false;
	addPostProcess("Fog", std::move(fogPp));
	// outline
	auto outlinePp = std::make_unique<PostProcess>();	// too lazy to make a dedicated class
	outlinePp->SetShader(std::make_unique<Shader>("res/shaders/pp_base_vertex.glsl", "res/shaders/pp_outline_fragment.glsl"));
	outlinePp->SetSettings(std::make_unique<Material>());	// u_FogDensity, u_FogStart, u_FogColor
	outlinePp->enabled = false;
	addPostProcess("Outline", std::move(outlinePp));
	// blur 
	auto blurPp = std::make_unique<BlurPP>();
	blurPp->enabled = false;
	addPostProcess("Blur", std::move(blurPp));
	// negative testing 
	auto negPp = std::make_unique<NegativePP>();
	negPp->enabled = false;
	addPostProcess("Negative", std::move(negPp));
	// bloom 
	auto bloomPp = std::make_unique<BloomPP>();
	addPostProcess("Bloom", std::move(bloomPp));

	// default skybox 
	std::vector<std::string> skyboxFaces = {
		"res/textures/cubemaps/skybox/right.tga",
		"res/textures/cubemaps/skybox/left.tga",
		"res/textures/cubemaps/skybox/up.tga",
		"res/textures/cubemaps/skybox/down.tga",
		"res/textures/cubemaps/skybox/back.tga",
		"res/textures/cubemaps/skybox/front.tga"
	};
	// auto skyboxTexId = Game::instance().loader.LoadCubemap(skyboxFaces);
	setSkybox(TextureLoader::LoadCubeMap(skyboxFaces));

	cpuProfiler.StopTimer(7);
}

RenderingSystem::~RenderingSystem()
{
	System::~System();
}

void RenderingSystem::SetSize(unsigned int width, unsigned int height)
{
	if (width == 0 || height == 0)
	{
		std::cerr << "ERROR: Screen size cannot be 0" << std::endl;
		return;
	}

	screenWidth = width;
	screenHeight = height;
	InitializeFrameBuffers();
	TextRenderer::Instance().SetScreenSize(width, height);
	uiProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -16000.0f, 16000.0f);
}

// todo: refactor with current system
void RenderingSystem::SetCamera(Camera * camera)
{
	this->activeCamera = camera;
}


void RenderingSystem::RenderGeometryPass()
{
	// 1. First pass - Geometry into gbuffers 
	// cleanup 
	ppWriteFBO->Draw();
	ppReadFBO->Draw();

	geometryShader->use();

	// bind geometry frame buffers 
	gbuffer->Draw();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// begin rendering each renderable 
	geometryShader->setMat4(SHADER_PROJECTION, projection);
	geometryShader->setMat4(SHADER_VIEW, view);
	
	// go thru each component 
	auto components = ComponentManager<RenderComponent>::Instance().All();
	
	for (auto& rc : components)
	{
		auto model = rc->GetEntity()->transform.getWorldTransformation();
		geometryShader->setMat4(SHADER_MODEL, model);
		
		// ensure default values incase material does not have it
		geometryShader->setVec3(SHADER_DIFFUSE, glm::vec3(1,1,1));
		geometryShader->setVec3(SHADER_SPECULAR, glm::vec3(1,1,1));
		geometryShader->setVec3(SHADER_AMBIENT, glm::vec3(1,1,1));

		// go thru each renderable package 
		for (int i = 0; i < rc->renderables.size(); i++)
		{
			auto r = rc->renderables[i];
			
			if (r->shader != nullptr)
			{
				// custom shader has been set
				r->shader->use();
				// ensure all uniforms have been set 
				r->shader->setMat4(SHADER_PROJECTION, projection);
				r->shader->setMat4(SHADER_VIEW, view);
				r->shader->setMat4(SHADER_MODEL, model);
				r->shader->setVec3(SHADER_DIFFUSE, glm::vec3(1, 1, 1));
				r->shader->setVec3(SHADER_SPECULAR, glm::vec3(1, 1, 1));
				r->shader->setVec3(SHADER_AMBIENT, glm::vec3(1, 1, 1));
				// TODO: Default loading (see comment below for issue).
				// auto freeTexSlot = r->shader->LoadDefaults(0);
				// freeTexSlot += 
				r->material->LoadMaterial(r->shader, 0);
			}
			else
			{
				geometryShader->LoadDefaults(0);	//OOF 
				r->material->LoadMaterial(geometryShader, 0); // (we load at 0 b/c we need to override the default texture)
			}

			// draw 
			glBindVertexArray(r->mesh->VAO);
			glDrawElements(GL_TRIANGLES, r->mesh->indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			if (r->shader != nullptr)
			{
				// switch back to default shader 
				geometryShader->use();
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingSystem::RenderShadowMapsPass()
{
	// 2nd Pass - lighting shadow map   
	shadowmapShader->use();

	auto components = ComponentManager<DirectionalLight>::Instance().All();
	auto render_components = ComponentManager<RenderComponent>::Instance().All();
	for (auto& dl : components)
	{
		shadowmapShader->setMat4(SHADER_LIGHTSPACE, dl->getLightSpaceMatrix());
		
		dl->PrepareShadowmap(shadowmapShader);

		// go thru each component 
		for (auto& rc : render_components)
		{
			shadowmapShader->setMat4(SHADER_MODEL, rc->GetEntity()->transform.getWorldTransformation());

			// go thru each renderable package 
			for (int i = 0; i < rc->renderables.size(); i++)
			{
				auto r = rc->renderables[i];
				glBindVertexArray(r->mesh->VAO);
				glDrawElements(GL_TRIANGLES, r->mesh->indices.size(), GL_UNSIGNED_INT, 0);
			}
		}
	}
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderingSystem::RenderAmbientLightingPass()
{
	auto lights = ComponentManager<AmbientLight>::Instance().All();
	
	if (lights.size() == 0)
		return;

	// prepare data for drawing 
	glm::vec3 addition = glm::vec3();
	for (auto& al : lights)
		addition += al->color * al->intensity;
	
	// draw 
	ambientLightShader->use();

	gbuffer->Read(ambientLightShader);
	ppWriteFBO->Draw(false);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	ambientLightShader->setVec3(SHADER_LIGHT_COLOR, addition);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void RenderingSystem::RenderDirectionalLightingPass()
{
	// 3rd pass - composision

	directionalLightShader->use();

	// don't render to backbuffer directly 
	ppWriteFBO->Draw(false);	// this if the first lighting pass - clear

	// read from gbuffer
	unsigned int availableTextureSlot =
		gbuffer->Read(directionalLightShader);

	// Apply directional lights ontop of each other 
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// settings for all lights
	directionalLightShader->setVec3(SHADER_VIEW_POS, activeCamera->GetEntity()->transform.getWorldPosition());
	directionalLightShader->setInt(SHADER_FB_SHD, availableTextureSlot);

	auto dlights = ComponentManager<DirectionalLight>::Instance().All();
	for (auto& dl : dlights)
	{
		directionalLightShader->setMat4(SHADER_LIGHTSPACE, dl->getLightSpaceMatrix());
		directionalLightShader->setVec3(SHADER_LIGHT_POS, dl->GetEntity()->transform.getWorldPosition());
		directionalLightShader->setVec3(SHADER_LIGHT_DIR, dl->GetEntity()->transform.getWorldForward());
		directionalLightShader->setVec3(SHADER_LIGHT_COLOR, dl->color);
		directionalLightShader->setFloat(SHADER_LIGHT_AMBIENT_INTENSITY, dl->ambientIntensity);
		directionalLightShader->setFloat(SHADER_SHADOW_NEAR, dl->shadowMapNear);
		directionalLightShader->setFloat(SHADER_SHADOW_NEAR, dl->shadowMapFar);

		glActiveTexture(GL_TEXTURE0 + availableTextureSlot);
		glBindTexture(GL_TEXTURE_2D, dl->GetShadowMapID());

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderingSystem::RenderPointLightingPass()
{
	pointLightShader->use();

	// don't render to backbuffer directly 
	ppWriteFBO->Draw(false);

	// read from gbuffer
	unsigned int availableTextureSlot =
		gbuffer->Read(pointLightShader);

	// compPLightShader->setInt(SHADER_FB_SHD, availableTextureSlot);
	pointLightShader->setVec3(SHADER_VIEW_POS, activeCamera->GetEntity()->transform.getWorldPosition());
	// we're rendering from camera this time - using light volumes
	pointLightShader->setMat4(SHADER_PROJECTION, projection);
	pointLightShader->setMat4(SHADER_VIEW, view);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	auto plights = ComponentManager<PointLight>::Instance().All();
	for (auto& pl : plights)
	{
		pointLightShader->setVec3(SHADER_LIGHT_POS, pl->GetEntity()->transform.getWorldPosition());
		pointLightShader->setVec3(SHADER_LIGHT_COLOR, pl->color);
		pointLightShader->setFloat(SHADER_LIGHT_INTENSITY, pl->intensity);
		pointLightShader->setFloat(SHADER_LIGHT_RANGE, pl->range);

		// scale model matrix (for light volume) one more time for light range 
		//auto model = glm::scale(
		//	pl->GetEntity()->transform.getWorldTransformation(),
		//	glm::vec3(pl->range));
		//pointLightShader->setMat4(SHADER_MODEL, model);

		// we're using a cube instead of a sphere b/c of less vertices 
		//glBindVertexArray(cubeVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	glBindVertexArray(0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderingSystem::RenderSpotLightingPass()
{
	spotLightShader->use();

	// don't render to backbuffer directly 
	ppWriteFBO->Draw(false);

	// read from gbuffer
	unsigned int availableTextureSlot =
		gbuffer->Read(spotLightShader);

	// compPLightShader->setInt(SHADER_FB_SHD, availableTextureSlot);
	spotLightShader->setVec3(SHADER_VIEW_POS, activeCamera->GetEntity()->transform.getWorldPosition());
	// we're rendering from camera this time - using light volumes
	spotLightShader->setMat4(SHADER_PROJECTION, projection);
	spotLightShader->setMat4(SHADER_VIEW, view);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	auto lights = ComponentManager<SpotLight>::Instance().All();
	for (auto& pl : lights)
	{
		spotLightShader->setVec3(SHADER_LIGHT_POS, pl->GetEntity()->transform.getWorldPosition());
		spotLightShader->setVec3(SHADER_LIGHT_DIR, pl->GetEntity()->transform.getWorldForward());
		spotLightShader->setVec3(SHADER_LIGHT_COLOR, pl->color);
		spotLightShader->setFloat(SHADER_LIGHT_INTENSITY, pl->intensity);
		spotLightShader->setFloat(SHADER_LIGHT_RANGE, pl->range);
		spotLightShader->setFloat(SHADER_LIGHT_ANGLE, glm::radians(pl->angle));


		// scale model matrix (for light volume) one more time for light range 
		//auto model = glm::scale(
		//	pl->GetEntity()->transform.getWorldTransformation(),
		//	glm::vec3(pl->range));
		//spotLightShader->setMat4(SHADER_MODEL, model);

		// we're using a cube instead of a sphere b/c of less vertices 
		//glBindVertexArray(cubeVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	glBindVertexArray(0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// 3.5th pass - skybox pass (should be relatively cheap).
void RenderingSystem::RenderSkyboxPass()
{
	// skybox pass 
	if (skyboxTexture)
	{
		// set render target
		ppWriteFBO->Draw(false);
		// load settings 
		// glDisable(GL_DEPTH_TEST);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		auto shader = (customSkyboxShader) ? customSkyboxShader : skyboxShader;
		shader->use();
		shader->setMat4(SHADER_PROJECTION, projection);
		shader->setMat4(SHADER_VIEW, glm::mat4(glm::mat3(view)));	// view -> mat3 (remove translation) -> mat4 (compatable format)
		if (skyboxSettings)
			skyboxSettings->LoadMaterial(shader, 0);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->GetId());
		// draw 
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		// we're going to manually revert the depth test (b/c we don't want every single other pass doing it)
		glDepthFunc(GL_LESS);
	}
}

void RenderingSystem::RenderPostProcessPass()
{
	// post processing 
	glDisable(GL_DEPTH_TEST);

	// setup current rendered image to read from
	std::swap(ppWriteFBO, ppReadFBO);

	for (auto& pp : _postProcessOrder)
	{
		if (!pp->enabled) continue;

		bool finished = false;

		while (!finished)
		{
			unsigned int freeTexSlot = 0;

			// Load shader
			pp->GetActiveShader()->use();

			// Load gbuffer textures if requested 
			if (pp->RequestingGBuffer())
			{
				freeTexSlot += gbuffer->Read(pp->GetActiveShader());
			}

			// Load back-buffer texture if requested
			if (pp->RequestingBackBuffer())
			{
				pp->GetActiveShader()->setInt("u_FinTex", freeTexSlot);
				freeTexSlot += ppReadFBO->Read(freeTexSlot);
			}

			// Set render target if requested 
			if (pp->DrawingToBackBuffer())
			{
				ppWriteFBO->Draw();
				std::swap(ppWriteFBO, ppReadFBO);	// swap finished texture so next PP can read/write to current image
			}

			// Final setup for this post-process pass 
			finished = pp->Pass(freeTexSlot);

			// render 
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render to back buffer
	glBlitNamedFramebuffer(ppReadFBO->GetFboId(), 0, 
		0, 0, screenWidth, screenHeight, 
		0, 0, screenWidth, screenHeight, 
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	// all remaining passes (UI) render directly to back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingSystem::RenderUIImagesPass()
{
	// 5th pass - UI images 
	profiler.StartTimer(4);
	cpuProfiler.StartTimer(4);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	auto uis = ComponentList<UIComponent>::Instance().All();
	std::sort(uis.begin(), uis.end(), UIComponent::CompareZOrder);

	for (auto& ui : uis)
	{
		if (!ui->GetActive())
			continue;

		auto img = dynamic_cast<UIImage*>(ui);
		if (img)
		{
			imageShader->use();
			auto size = glm::vec2(img->screenBounds.getWidth(), img->screenBounds.getHeight());
			auto transform = img->GetTransform() * img->GetIndividualTransform();

			imageShader->setMat4(SHADER_PROJECTION, uiProjection);
			imageShader->setMat4("u_Model", transform);
			imageShader->setVec4("u_Tint", img->color.vec4());

			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, img->GetTexture()->GetId());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}

		auto txt = dynamic_cast<UIText*>(ui);
		if (txt)
		{
			TextRenderer::Instance().RenderText(
				txt->GetTextMesh(),
				txt->GetTransform() * txt->GetIndividualTransform(), 
				txt->color.vec4());
		}
	}

	profiler.StopTimer(4);
	cpuProfiler.StopTimer(4);
}

void RenderingSystem::RenderUITextPass()
{
}

void RenderingSystem::RenderCompositionPass()
{
}


void RenderingSystem::DrawComponent(RenderComponent* component)
{
	glm::mat4 model = component->GetEntity()->transform.getWorldTransformation();

	for (int i = 0; i < component->renderables.size(); i++)
	{
		auto r = component->renderables[i];
	}
}

// initializes a framebuffer object for deferred rendering
void RenderingSystem::InitializeFrameBuffers()
{
	gbuffer = new GBuffer();
	ppReadFBO = new FrameBuffer();
	ppWriteFBO = new FrameBuffer();
}

void RenderingSystem::InitializeScreenQuad()
{
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);	// remember this syntax only works here b/c we defined the array in the same scope 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void RenderingSystem::InitializeScreenCube()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);	// remember this syntax only works here b/c we defined the array in the same scope 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RenderingSystem::Update(float dt)
{
	cpuProfiler.StartTimer(0);

	// Cleanup 
	glClearDepth(1.0f);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// check for valid state 
	if (screenWidth == 0 || screenHeight == 0)
	{
		std::cerr << "ERROR: Screen dimensions are 0" << std::endl;
		return;
	}
	//else if (_cameras.size() == 0)
	//{
	//	std::cerr << "ERROR: There are no cameras" << std::endl;
	//	return;
	//}

	// use first main camera
	SetCamera(nullptr);
	auto _cams = ComponentManager<Camera>::Instance().All();
	for (auto& c : _cams)
	{
		if (c->isMain)
		{
			SetCamera(c);
			break;
		}
	}
	//for (int i = 0; i < _cameras.size(); i++)
	//{
	//	if (_cameras[i]->isMain)
	//	{
	//		SetCamera(_cameras[i]);
	//		break;
	//	}
	//}
	if (activeCamera == nullptr)
	{
		std::cerr << "ERROR: No camera has been set for rendering" << std::endl;
		return;
	}


	// Render 

	// prepare data 
	projection = activeCamera->GetProjectionMatrix();
	view = activeCamera->GetViewMatrix();

	cpuProfiler.StopTimer(0);

	// draw everything
	profiler.StartTimer(1);
	cpuProfiler.StartTimer(1);
	RenderGeometryPass();				// 1st pass (geometry)
	profiler.StopTimer(1);
	cpuProfiler.StopTimer(1);

	profiler.StartTimer(2);
	cpuProfiler.StartTimer(2);
	RenderShadowMapsPass();				// 2nd pass (shadow maps)
	profiler.StopTimer(2);
	cpuProfiler.StopTimer(2);

	RenderAmbientLightingPass();

	profiler.StartTimer(3);
	cpuProfiler.StartTimer(3);
	RenderDirectionalLightingPass();	// 3rd pass (lighting)
	profiler.StopTimer(3);
	cpuProfiler.StopTimer(3);

	profiler.StartTimer(4);
	cpuProfiler.StartTimer(4);
	RenderPointLightingPass();			// 4rd pass (lighting)
	profiler.StopTimer(4);
	cpuProfiler.StopTimer(4);

	RenderSpotLightingPass();

	profiler.StartTimer(5);
	cpuProfiler.StartTimer(5);
	RenderSkyboxPass();					// 5th pass (skybox)
	profiler.StopTimer(5);
	cpuProfiler.StopTimer(5);

	profiler.StartTimer(6);
	cpuProfiler.StartTimer(6);
	RenderPostProcessPass();			// 6th pass (post processes)
	profiler.StopTimer(6);
	cpuProfiler.StopTimer(6);

	profiler.StartTimer(7);
	cpuProfiler.StartTimer(7);
	RenderUIImagesPass();				// 7th pass (UI)
	profiler.StopTimer(7);
	cpuProfiler.StopTimer(7);

	profiler.StartTimer(8);
	cpuProfiler.StartTimer(8);
	RenderUITextPass();					// 8th pass (UI)
	profiler.StopTimer(8);
	cpuProfiler.StopTimer(8);

	profiler.FrameFinish();
	cpuProfiler.FrameFinish();

	// render debug info (please note that this is pretty performance heavy ~2ms)
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2)
		<< "Geometry Pass: " << profiler.GetDuration(1) / 1000000.0 << "ms\n"
		<< "Shadowmap Pass: " << profiler.GetDuration(2) / 1000000.0 << "ms\n"
		<< "Directional Lighting Pass: " << profiler.GetDuration(3) / 1000000.0 << "ms\n"
		<< "Point Lighting Pass: " << profiler.GetDuration(4) / 1000000.0 << "ms\n"
		<< "Skybox Pass: " << profiler.GetDuration(5) / 1000000.0 << "ms\n"
		<< "Postprocessing Pass: " << profiler.GetDuration(6) / 1000000.0 << "ms\n"
		<< "Image Pass: " << profiler.GetDuration(7) / 1000000.0 << "ms\n"
		<< "Text Pass: " << profiler.GetDuration(8) / 1000000.0 << "ms\n";

	// TextRenderer::Instance().RenderText(ss.str(), screenWidth, screenHeight, glm::vec4(1.0f), 0.5f, TextAlignment::Right);
}

void RenderingSystem::addPostProcess(const std::string name, std::unique_ptr<PostProcess> postProcess)
{
	_postProcesses[name] = std::move(postProcess);
	_postProcessOrder.push_back(_postProcesses[name].get());
	std::sort(_postProcessOrder.begin(), _postProcessOrder.end(), PostProcess::ComparePriority);
}

void RenderingSystem::removePostProcess(const std::string name)
{
	auto p = _postProcesses.find(name);
	if (p != _postProcesses.end())
	{
		auto orderIdx = std::find(_postProcessOrder.begin(), _postProcessOrder.end(), p->second.get());
		_postProcessOrder.erase(orderIdx);
		_postProcesses.erase(name);
	}
}

PostProcess* RenderingSystem::getPostProcess(const std::string name)
{
	auto pp = _postProcesses.find(name);
	return (pp != _postProcesses.end()) ? pp->second.get() : nullptr;
}

void RenderingSystem::setSkybox(CubeMap* cubemap)
{
	skyboxTexture = cubemap;
}

void RenderingSystem::setSkyboxShader(Shader * shader)
{
	customSkyboxShader = shader;
}

void RenderingSystem::setSkyboxSettings(Material * material)
{
	skyboxSettings = material;
}

//void RenderingSystem::onComponentCreated(std::type_index t, Component* c)
//{
//	if (t == std::type_index(typeid(RenderComponent)))
//	{
//		std::cout << "RenderingSystem adding render component" << std::endl;
//		_components.push_back(static_cast<RenderComponent*>(c));
//	}
//	else if (t == std::type_index(typeid(DirectionalLight)))
//	{
//		std::cout << "RenderingSystem adding light" << std::endl;
//		_dlights.push_back(static_cast<DirectionalLight*>(c));
//	}
//	else if (t == std::type_index(typeid(Camera)))
//	{
//		std::cout << "RenderingSystem adding camera" << std::endl;
//		_cameras.push_back(static_cast<Camera*>(c));
//	}
//}
//
//void RenderingSystem::onComponentDestroyed(std::type_index t, Component* c)
//{
//	if (t == std::type_index(typeid(RenderComponent)))
//	{
//		auto target = std::find(_components.begin(), _components.end(), c);
//		if (target != _components.end())
//			_components.erase(target);
//		else
//			std::cerr << "ERROR: RenderingSystem removing component but couldn't find it!" << std::endl;
//	}
//	else if (t == std::type_index(typeid(DirectionalLight)))
//	{
//		auto target = std::find(_dlights.begin(), _dlights.end(), c);
//		if (target != _dlights.end())
//			_dlights.erase(target);
//		else
//			std::cerr << "ERROR: RenderingSystem removing component but couldn't find it!" << std::endl;
//	}
//	else if (t == std::type_index(typeid(Camera)))
//	{
//		std::cout << "Camera destroyed" << std::endl;
//		auto target = std::find(_cameras.begin(), _cameras.end(), c);
//		if (target != _cameras.end())
//			_cameras.erase(target);
//		else
//			std::cerr << "ERROR: RenderingSystem removing component but couldn't find it!" << std::endl;
//	}
//}

/* Notes:

	// create depth buffer (you could also make this a texture - if you need to read from it)
	//unsigned int RBO;
	//glGenRenderbuffers(1, &RBO);
	//glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// more texture parameters you might need
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

*/


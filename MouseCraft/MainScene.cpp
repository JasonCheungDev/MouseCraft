#include "MainScene.h"

#include "Core/EntityManager.h"
#include "ComponentFactory.h"
#include "Rendering/CubeMesh.h"
#include "Rendering/RenderComponent.h"
#include "Rendering/Camera.h"
#include "Rendering/Lighting/DirectionalLight.h"
#include "Rendering/UI/ImageComponent.h"

void MainScene::InitScene() 
{
	auto cubeMesh = std::make_shared<CubeMesh>();

	auto defaultMaterial = std::make_shared<Material>();

	auto renderable = std::make_shared<Renderable>();
	renderable->mesh = cubeMesh;
	renderable->material = defaultMaterial;

	auto c_render = ComponentFactory::Create<RenderComponent>();
	c_render->addRenderable(renderable);

	auto e_test = EntityManager::Instance().Create();
	e_test->AddComponent(c_render);
	e_test->transform.setLocalPosition(glm::vec3(1.0f, 0, -5.0f));
	e_test->transform.setLocalRotation(glm::vec3(0.1f, 0.2f, 0.3f));

	auto c_cam = ComponentFactory::Create<Camera>();
	auto e_cam = EntityManager::Instance().Create();
	e_cam->AddComponent(c_cam);

	auto c_light = ComponentFactory::Create<DirectionalLight>();
	auto e_light = EntityManager::Instance().Create();
	e_light->AddComponent(c_light);
	e_light->transform.setLocalRotation(glm::vec3(0.42f, 0, 0));
	
	auto c_img = ComponentFactory::Create<ImageComponent>();
	c_img->loadImage("res/textures/araragi_karen.png");
	auto e_img = EntityManager::Instance().Create();
	e_img->AddComponent(c_img);

	root.AddChild(e_test);
	root.AddChild(e_cam);
	root.AddChild(e_light);
	root.AddChild(e_img);
}

void MainScene::Update(const float delta) 
{

}

void MainScene::CleanUp() 
{

}
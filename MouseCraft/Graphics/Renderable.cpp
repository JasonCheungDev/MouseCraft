#include "Renderable.h"
#include "ModelGen.h"
#include "../Core/Entity.h"
#include "../Loading/ModelLoader.h"
#include "../Loading/ImageLoader.h"
#include "../ResourceCache.h"
#include <string>
#include <sstream>

Renderable::Renderable() :
	_shininess(0.5),
	_smoothness(25.0)
{}

Model* Renderable::getModel() {
	return _model;
}

void Renderable::setModel(Model& model) {
	_model = &model;
}

Transform Renderable::getTransform() {
	Entity* e = GetEntity();
	if (e != nullptr) {
		return e->transform;
	}
	return Transform();
}

Color Renderable::getColor() {
	return _color;
}

void Renderable::setColor(Color color) {
	_color = color;
}

void Renderable::setShininess(float f) {
	_shininess = f;
}

float Renderable::getShininess() {
	return _shininess;
}

void Renderable::setRoughness(float f) {
	_smoothness = f;
}

float Renderable::getSmoothness() {
	return _smoothness;
}


#pragma region prefab support 

Component* Renderable::CreateFromJson(json json)
{
	auto c = ComponentManager<Renderable>::Instance().Create<Renderable>();

	// parse shininess
	if (json.find("shininess") != json.end())
	{
		c->_shininess = json["shininess"].get<double>();
	}
	
	// parse smoothness
	if (json.find("smoothness") != json.end())
	{
		c->_smoothness = json["smoothness"].get<double>();
	}

	// parse color 
	if (json.find("color") != json.end())
	{
		c->_color = Color(
			json["color"]["r"].get<double>(),
			json["color"]["g"].get<double>(),
			json["color"]["b"].get<double>());
	}

	// parse geometry and texture
	if (json.find("model_gen") != json.end())
	{
		// use model generation  
		if (json["model_gen"]["type"].get<std::string>() == "cube")
		{
			auto& jval = json["model_gen"]["size"];
			c->_model = ModelGen::makeCube(jval[0].get<float>(), jval[1].get<float>(), jval[2].get<float>());
		}
		else
		{
			std::cerr << "ERROR: Renderable JSON ctor failed to generate model for unknown type." << std::endl;
		}
	}
	else
	{
		// use model loading 
		std::string mPath = json["model_path"].get<std::string>();
		std::string tPath = json["texture_path"].get<std::string>();
		std::string key = mPath + tPath;

		// use cached model if possible 
		Model* model = ResourceCache<Model>::Instance().Get(key);
		if (model == nullptr)
		{
			model = ModelLoader::loadModel(mPath);
			if (tPath.size() > 0)
				model->setTexture(new std::string(tPath));
			ResourceCache<Model>::Instance().Add(key, model);
		}
		c->_model = model;

	}

	return c;
}

PrefabRegistrar Renderable::reg("Renderable", &Renderable::CreateFromJson);

#pragma endregion
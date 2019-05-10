#include "ModelLoader.h"
#include <fstream>
#include <iostream>

using std::ifstream;
using std::string;
using std::vector;

std::string ModelLoader::directory = "";

Entity * ModelLoader::LoadFromJson(json json)
{
	return ModelLoader::Load(json["path"].get<std::string>());
}

EntityRegistrar ModelLoader::reg("model", &ModelLoader::LoadFromJson);
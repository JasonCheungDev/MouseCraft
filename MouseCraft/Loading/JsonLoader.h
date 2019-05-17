#pragma once

#include <iostream>
#include <fstream>
#include <json.hpp>
#include "ResourceCache.h"
using json = nlohmann::json;

class JsonLoader
{
public:
	// Loads json from path.
	// Will return a preloaded json object if found. 
	static json* Load(const std::string& path)
	{
		auto j = ResourceCache<json>::Instance().Get(path);
		
		if (j == nullptr)
		{
			j = LoadCopy(path);
			ResourceCache<json>::Instance().Add(path, j);
		}

		return j;
	}

	// Loads json from path.
	// Recommended to use Load() instead. 
	static json* LoadCopy(const std::string& path)
	{
		auto j = new json();

		// open file and parse json 
		std::ifstream ifs(path);
		if (!ifs.good())
		{
			std::cerr << "ERROR: JsonLoader could not find file: " << path << std::endl;
			return nullptr;
		}
		ifs >> *j;

		return j;
	}
};


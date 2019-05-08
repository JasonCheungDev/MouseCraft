#pragma once

#include <vector>
#include <algorithm>
#include "Core/Component.h"

class IComponentMan
{
public:
	virtual void Delete(int id) = 0;
};

template<class ComponentType>
class ComponentMan : public IComponentMan
{
// singleton 
public:
	static ComponentMan& Instance()
	{
		static ComponentMan _instance;
		return _instance;
	}
	ComponentMan(ComponentMan const&) = delete;
	void operator=(ComponentMan const&) = delete;
private:
	ComponentMan() {};
	~ComponentMan() {};

// functions 
public:
	template<typename... Args>
	ComponentType* Create(Args... args)
	{
		auto c = new ComponentType(args...);
		_data.push_back(c);
		return c;
	}

	void Delete(int id) override
	{
		auto it = std::find_if(_data.begin(), _data.end(), [id](ComponentType* c) { 
			return static_cast<Component*>(c)->GetID() == id; 
		});
		delete(*it);
		_data.erase(it);
	}

	const std::vector<ComponentType*>& All()
	{
		return _data;
	}

// variables 
private:
	// Current strategy is to use pointer array.
	// This is to ensure pointers aren't invalidated.
	std::vector<ComponentType*> _data;

	// Consider: A double layered array, 1st array with continuous gapless data
	// 2nd array with Handle pointers that point to continuous data.
	// Whenever a component is destroyed the last element in the 1st array takes that spot,
	// and the handle internal pointer is updated to point to that new spot. 
	
	// Be aware that array of pointers is fine so far (performs better or just as good as 
	// continuous objects in MultiThreadTest project). Most likely need larger and more 
	// fragmented data for cache efficiency to shine. 
};


#pragma once

#include <vector>
#include "Core/Component.h"

class IComponentMan
{
public:
	virtual void Delete(int id) = 0;
};

template<class ComponentType>
class ComponentMan : IComponentMan
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
	ComponentMan()
	{
	};
	~ComponentMan() {};

public:
	ComponentType* Create()
	{
		_data.emplace_back();
		return &_data[_data.size() - 1];
	}

	const std::vector<ComponentType>& All()
	{
		return _data;
	}

private:
	std::vector<ComponentType> _data;
};


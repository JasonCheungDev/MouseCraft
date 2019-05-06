#pragma once

#include <vector>
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

public:
	ComponentType* Create()
	{
		_data.push_back(ComponentType());

		auto something = &_data[_data.size() - 1];

		return &_data[_data.size() - 1];
	}

	void Delete(int id) override
	{
		// Current strategy is to lazy delete a component. 
		// This is to ensure no pointers to a specific component are invalidated. 
		for (auto& c : _data)
		{
			auto component = static_cast<Component*>(&c);
			if (component->GetID() == id)
			{
				component->SetDeletedFlag(true);
				break;
			}
		}
	}

	const std::vector<ComponentType>& All()
	{
		return _data;
	}

private:
	std::vector<ComponentType> _data;
};


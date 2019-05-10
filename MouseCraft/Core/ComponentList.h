#pragma once

#include <vector>
#include "ComponentFactory.h"
#include "Component.h"
#include "Handle.h"
#include "../Event/EventManager.h"

template<class ComponentType>
class ComponentList : public ISubscriber
{
// singleton 
public:
	static ComponentList& Instance()
	{
		static ComponentList _instance;
		return _instance;
	}
	ComponentList(ComponentList const&) = delete;
	void operator=(ComponentList const&) = delete;
private:
	ComponentList()
	{
		for (auto& kvp : ComponentFactory::Instance().GetComponentDirectory())
		{
			auto c = dynamic_cast<ComponentType*>(kvp.first);
			if (c) _data.push_back(c);
		}
		EventManager::Subscribe(EventName::COMPONENT_ADDED, this);
	};
	~ComponentList()
	{
		EventManager::Unsubscribe(EventName::COMPONENT_REMOVED, this);
	};

// functions
public:
	const std::vector<ComponentType*>& All()
	{
		return _data;
	}

	virtual void Notify(EventName name, Param* params) override
	{
		if (name == EventName::COMPONENT_ADDED)
		{
			auto* component = static_cast<TypeParam<Component*>*>(params)->Param;
			auto myType = dynamic_cast<ComponentType*>(component);
			if (myType) 
			{
				_data.push_back(myType);
			}
		}
		else if (name == EventName::COMPONENT_REMOVED)
		{
			auto component = static_cast<TypeParam<Component*>*>(params)->Param;
			auto myType = dynamic_cast<ComponentType*>(component);
			if (myType)
			{
				auto it = std::find(_data.begin(), _data.end(), myType);
				if (it != _data.end()) _data.erase(it);
			}
		}
	}

// variables
private:
	std::vector<ComponentType*> _data;
};


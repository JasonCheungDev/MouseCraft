#pragma once

#include <map>
#include "Core/Component.h"
#include "Event/EventManager.h"
#include "ComponentMan.h"

class ComponentFactory
{
public:
	ComponentFactory();
	~ComponentFactory();

	template<typename ComponentType>
	ComponentType* Create()
	{
		auto newComponent = ComponentMan<ComponentType>::Instance().Create();
		// raise event !
		return newComponent;
	}

private:
	
};


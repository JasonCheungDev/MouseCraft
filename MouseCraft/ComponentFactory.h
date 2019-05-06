#pragma once

#include <map>
#include "Core/Component.h"
#include "Event/EventManager.h"
#include "ComponentMan.h"

class ComponentFactory
{
// singleton 
public:
	static ComponentFactory& Instance()
	{
		static ComponentFactory _instance;
		return _instance;
	}
	// WARNING!:
	ComponentFactory(ComponentFactory const&) = delete;
	void operator=(ComponentFactory const&) = delete;
private:
	ComponentFactory() {};
	~ComponentFactory() {};

// static alias
public:
	template<typename ComponentType>
	static ComponentType* Create()
	{
		return Instance().CreateComponent<ComponentType>();
	}

	void Destroy(Component* c)
	{
		Instance().DestroyComponent(c);
	}

// functions
public:
	template<typename ComponentType>
	ComponentType* CreateComponent()
	{
		// create component 
		auto newComponent = ComponentMan<ComponentType>::Instance().Create();
		
		// add to directory 
		_componentDirectory[newComponent] = &ComponentMan<ComponentType>::Instance();

		// raise event
		EventManager::Notify(EventName::COMPONENT_ADDED, new TypeParam<Component*>(newComponent));
		
		return newComponent;
	}

	void DestroyComponent(Component* c)
	{
		// raise event 
		EventManager::Notify(EventName::COMPONENT_REMOVED, new TypeParam<Component*>(c));

		// remove from directory 
		_componentDirectory.erase(c);

		// release memory 
		_componentDirectory[c]->Delete(c->GetID());
	}

	// returns the component directory. only call this if you know what you're doing.
	const std::map<Component*, IComponentMan*>& GetComponentDirectory()
	{
		return _componentDirectory;
	}

// variables 
private:
	// points to the IComponentManager that is managing Component with ID. 
	std::map<Component*, IComponentMan*> _componentDirectory;
};


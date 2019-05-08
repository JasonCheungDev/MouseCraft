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
	template<typename ComponentType, typename... Args>
	static ComponentType* Create(Args... args)
	{
		return Instance().CreateComponent<ComponentType>(args...);
	}

	void Destroy(Component* c)
	{
		Instance().DestroyComponent(c);
	}

// functions
public:
	template<typename ComponentType, typename... Args>
	ComponentType* CreateComponent(Args... args)
	{
		// create component 
		auto newComponent = ComponentMan<ComponentType>::Instance().Create(args...);
		
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

		// release memory 
		_componentDirectory[c]->Delete(c->GetID());

		// remove from directory 
		_componentDirectory.erase(c);
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


#include "Component.h"
#include <iostream>
#include "../Event/EventManager.h"
#include "ComponentManager.h"
#include "Entity.h"

unsigned int Component::_curID = 0;

Component::Component() : _id(Component::_curID++)
{
	// std::cout << "Component created" << std::endl;
	//TypeParam<Component*> param(this);
	//EventManager::Notify(COMPONENT_ADDED, &param, false);
}

Component::~Component()
{
	// std::cout << "Component destroyed" << std::endl;
	TypeParam<Component*> param(this);
	EventManager::Notify(COMPONENT_REMOVED, &param, false);
}

// copy ctor
Component::Component(const Component& c) 
{
	// update memory location 
	_self = c._self;
	_self->Set(this);	
	// copy configuration
	_id = c._id;
	_destroyed = c._destroyed;
	_initialized = c._initialized;
	_enabled = c._enabled;
	_entity = c._entity;
}

// Note: Looks kind of strange but this is to prevent components
// from being initialized twice and hides this interaction 
// away from implementing classes.
// Note: Implementors should extend onInitialized() 
// Consider: Deleting this, as it's currently handled by Entity. (maybe?) 
void Component::Initialize()
{
	if (_initialized)
	{
		std::cerr << "WARNING: Component tried to init twice - how did that happen?" << std::endl;
		return;
	}
	_initialized = true;
	OnInitialized();
}

bool Component::GetActive() const
{
	return !GetDeletedFlag() && GetEnabled() && GetEntity() && GetEntity()->GetActive();
}

#include "Component.h"
#include <iostream>
#include "../Event/EventManager.h"
#include "Entity.h"

unsigned int Component::_curID = 0;

Component::Component() : _id(Component::_curID++)
{
	// std::cout << "Component created" << std::endl;
}

Component::~Component()
{
	// std::cout << "Component destroyed" << std::endl;
}

// Note: Looks kind of strange but this is to prevent components
// from being initialized twice and hides this interaction 
// away from implementing classes.
// Note: Implementors should extend onInitialized() 
// Consider: Deleting this, as it's currently handled by Entity. (maybe?) 
void Component::Initialize()
{
	if (_initialized) return;
	_initialized = true;
	OnInitialized();
}

bool Component::GetActive() const
{
	return GetEnabled() && GetEntity() && GetEntity()->GetActive();
}

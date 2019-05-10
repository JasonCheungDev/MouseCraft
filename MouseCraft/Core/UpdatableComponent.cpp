#include "UpdatableComponent.h"

#include "../Event/EventManager.h"
#include "Entity.h"

UpdatableComponent::UpdatableComponent()
{
	EventManager::Subscribe(COMPONENT_F_UPDATE, this);
	EventManager::Subscribe(COMPONENT_UPDATE, this);
}

UpdatableComponent::~UpdatableComponent()
{
	EventManager::Unsubscribe(COMPONENT_F_UPDATE, this);
	EventManager::Subscribe(COMPONENT_UPDATE, this);
}

void UpdatableComponent::Notify(EventName eventName, Param * params)
{
	if (eventName == EventName::COMPONENT_F_UPDATE 
		&& GetEnabled() && GetEntity() && GetEntity()->GetActive())
	{	
		auto delta = static_cast<TypeParam<std::pair<float, int>>*>(params)->Param;
		FixedUpdate(delta.first, delta.second);
	}
	else if (eventName == EventName::COMPONENT_UPDATE
		&& GetEnabled() && GetEntity() && GetEntity()->GetActive())
	{
		auto delta = static_cast<TypeParam<float>*>(params);
		Update(delta->Param);
	}
}

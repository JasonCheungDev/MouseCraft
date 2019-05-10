#include "HealthComponent.h"
#include "../Core/ComponentFactory.h"

Component* HealthComponent::Create(json json)
{
	auto c_health = ComponentFactory::Create<HealthComponent>(); 
	c_health->_health = json["hp"].get<int>();
	c_health->_shield = json["shield"].get<bool>();
	return c_health;
}

ComponentRegistrar HealthComponent::reg("Health", &HealthComponent::Create);
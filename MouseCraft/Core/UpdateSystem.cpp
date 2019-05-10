#include "UpdateSystem.h"

#include "ComponentList.h"
#include "UpdatableComponent.h"

UpdateSystem::UpdateSystem()
{
}

UpdateSystem::~UpdateSystem()
{
}

void UpdateSystem::Update(float dt)
{
	auto& components = ComponentList<UpdatableComponent>::Instance().All();
	for (auto& uc : components)
		if (uc->GetActive())
			uc->Update(dt);
}

void UpdateSystem::FixedUpdate(float dt, int steps)
{
	auto& components = ComponentList<UpdatableComponent>::Instance().All();
	for (auto& uc : components)
		if (uc->GetActive())
			uc->FixedUpdate(dt, steps);
}

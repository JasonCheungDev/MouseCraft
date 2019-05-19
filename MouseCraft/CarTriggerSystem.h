#pragma once

#include <glm/glm.hpp>
#include "Core/System.h"
#include "Core/ComponentManager.h"
#include "Car.h"
#include "TriggerZone.h"

class CarTriggerSystem : public System
{
public:
	void SetCar(Car* car) { _car = car; }

	virtual void FixedUpdate(float dt, int steps) override 
	{
		if (!_car) return;

		auto carPos = _car->GetEntity()->transform.getWorldPosition();

		auto& triggers = ComponentManager<TriggerZone>::Instance().All();
		for (auto& t : triggers)
		{
			// note: doesn't account for car bounds but is accurate enough
			if (glm::distance(t->GetEntity()->transform.getWorldPosition(), carPos) < t->radius)
			{
				if (!t->isInside)
				{
					std::cout << "TRIGGER ZONE ENTERED" << std::endl;
					t->OnTriggerEnter.Notify();
				}
				t->isInside = true;
			}
			else
			{
				if (t->isInside)
				{
					std::cout << "TRIGGER ZONE EXITED" << std::endl;
					t->OnTriggerExit.Notify();
				}
				t->isInside = false;
			}
		}
	};

private:
	Car* _car = nullptr;
};


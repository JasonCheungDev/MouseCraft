#pragma once

#include "Core/Component.h"
#include "Event/Subject.h"
#include "Car.h"

// Trigger zone for the player 
class TriggerZone : public Component
{
public:
	float radius = 5.0f;
	Subject<> OnTriggerEnter;
	Subject<> OnTriggerExit;

	// used by the engine - don't touhc
	bool isInside = false;
};


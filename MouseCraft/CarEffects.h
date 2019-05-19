#pragma once

#include "Car.h"
#include "TriggerZone.h"
#include "Common/TransformAnimator.h"
#include "Rendering/Lighting/PointLight.h"
#include "Event/Handler.h"

// This file contains a list of effects that can occur 

class CarEffects
{
public:
	CarEffects();
	~CarEffects();
};

class SpeedModifierEffect
{
public:
	SpeedModifierEffect(Car* car, float modifier = 0.0f) : _car(car), speedModChange(modifier) {}

	float speedModChange;

	void ActivateEffect()
	{
		if (!_activated)
		{
			_car->speedMod += speedModChange;
			_activated = true;
		}
	};

	void DeactivateEffect()
	{
		if (_activated)
		{
			_car->speedMod -= speedModChange;
			_activated = false;
		}
	};

	Handler<SpeedModifierEffect> ActivateHandler = Handler<SpeedModifierEffect>(this, &SpeedModifierEffect::ActivateEffect);
	Handler<SpeedModifierEffect> DeactivateHandler = Handler<SpeedModifierEffect>(this, &SpeedModifierEffect::DeactivateEffect);

private:
	Car* _car; 
	bool _activated = false;
};

// This is the controlling class for the Grand Gates zone.
// The player is shown signs that appear on the left or right, 
// which colored lights red or blue. Which side the sign appears 
// on indicates which path to take (blue = folow, red = opposite). 
class GrandGatesSequence
{
public:
	glm::vec3 blueLight = glm::vec3(0, 0, 1);
	glm::vec3 redLight = glm::vec3(1, 0, 0);
	float correctPathBoost = 0.2f;
	float incorrectPathBoost = -0.9f;

	// animators, lights, trigger zones (start, left, right, end), player
	GrandGatesSequence(
		Car* car,
		std::vector<TransformAnimator*> animations,
		std::vector<PointLight*> lights,
		TriggerZone* start,
		TriggerZone* left,
		TriggerZone* right,
		TriggerZone* end) 
		: _car(car), _animations(animations), _lights(lights)
	{
		start->OnTriggerEnter.Attach(ActivateHandler);
		left->OnTriggerEnter.Attach(OnTriggerLeft);
		right->OnTriggerEnter.Attach(OnTriggerRight);
		end->OnTriggerEnter.Attach(DeactivateHandler);
	}

	void Start() 
	{
		if (_phase != 0)
			return;
		_phase++;

		// Generate a new instance 
		_signsRight    = rand() % 2 == 0;	// should the signs move to the right or left? 
		_lightsCorrect = rand() % 2 == 0;	// should the lights indicate correct (blue) or incorrect (red)?
		_goRight       = _signsRight == _lightsCorrect;

		// setup the signs 
		for (auto& ta : _animations)
			ta->SetCurrentAnimation((_signsRight) ? "right" : "left");

		// setup the lights 
		for (auto& l : _lights)
			l->color = (_lightsCorrect) ? blueLight : redLight;
	}

	void EnteredLeftPath()
	{
		if (_phase != 1)
			return;
		_phase++;
		
		_currentBoost = (!_goRight) ? correctPathBoost : incorrectPathBoost;
		_car->speedMod += _currentBoost;
	}

	void EnteredRightPath()
	{
		if (_phase != 1)
			return;
		_phase++;

		_currentBoost = (_goRight) ? correctPathBoost : incorrectPathBoost;
		_car->speedMod += _currentBoost;
	}
	
	void End()
	{
		if (_phase != 2)
			return;
		_phase = 0;

		_car->speedMod -= _currentBoost;
	}

private: 
	bool _signsRight;
	bool _lightsCorrect;
	bool _goRight;
	float _currentBoost;
	std::vector<TransformAnimator*> _animations;
	std::vector<PointLight*> _lights; 
	Car* _car;
	int _phase = 0;

	Handler<GrandGatesSequence> ActivateHandler = Handler<GrandGatesSequence>(this, &GrandGatesSequence::Start);
	Handler<GrandGatesSequence> OnTriggerLeft = Handler<GrandGatesSequence>(this, &GrandGatesSequence::EnteredLeftPath);
	Handler<GrandGatesSequence> OnTriggerRight = Handler<GrandGatesSequence>(this, &GrandGatesSequence::EnteredRightPath);
	Handler<GrandGatesSequence> DeactivateHandler = Handler<GrandGatesSequence>(this, &GrandGatesSequence::End);
};

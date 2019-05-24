#pragma once

#include "../OmegaEngineDefines.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <climits>
#include <glm/glm.hpp>
#include <array>
#include "../Core/System.h"
#include "../Event/EventManager.h"
#include "../Util/CpuProfiler.h"

#ifndef MAX_PLAYERS
#define MAX_PLAYERS 4
#endif // !MAX_PLAYERS

#ifndef JOYSTICK_DEADZONE
#define JOYSTICK_DEADZONE 0.16
#endif // !JOYSTICK_DEADZONE

#ifndef DEBUG_PLAYER
#define DEBUG_PLAYER 10
#endif // !DEBUG_PLAYER

// Defines the possible axis this game supports.
// Note: Order matters, should always be Master, Horizontal, Vertical.
enum Axis
{
	STICK_LEFT,		
	STICK_LEFT_HOR,
	STICK_LEFT_VER,
	STICK_RIGHT,		
	STICK_RIGHT_HOR,
	STICK_RIGHT_VER,
	TRIGGER,
	TRIGGER_LEFT,
	TRIGGER_RIGHT
};

enum Button
{
	PRIMARY,	// R1				(idx5)
	SECONDARY,	// L1				(idx4)
	NORTH,		//					(idx3)
    EAST,		//					(idx2)
	SOUTH,		//					(idx0)
	WEST,		//					(idx1)
	OPTION,		// OPTION BUTTON	(idx6)
	UP,
	RIGHT,
	DOWN,
	LEFT
};

struct AxisEvent
{
	int player;
	Axis axis;
	float value;
};

struct Axis2DEvent
{
	int player;
	Axis axis;
	glm::vec2 value;

	// returns value normalized (length of 1)
	glm::vec2 GetDir()
	{
		return glm::normalize(value);
	}

	// returns value with length clamped between 0-1
	glm::vec2 GetClamped()
	{
		return (glm::length(value) > 1.0f) ? glm::normalize(value) * 1.0f : value;
	}
};

struct ButtonEvent
{
	int player;
	Button button;
	bool isDown;
};

struct MouseButtonEvent
{
	glm::ivec2 position;	// cursor position on screen 
	bool isRight;			// right or left mouse button.
	bool isDown;			// is button pressed down
};

struct KeyEvent
{
	SDL_Keycode key;
	bool isDown;
};



class InputSystem : public System
{
private:
	// Internal class that helps parse 2D axis input for SDL. 
	// This class handles deadzone filtering and determining 
	// if a value has logically changed. 
	// 
	// Instructions: 
	// 1. Optionally call SetX() and/or SetY()
	// 2. Always call Update() once per frame, after all input has been read. 
	// 3. Use HasXXXChanged() to check if input has logically changed
	// 4. Use GetXXX() to retrieve input
	class Axis2DInput
	{
	// variables
	private:
		// shared 
		float deadzone = JOYSTICK_DEADZONE;
		float rawX;					// raw current value
		float rawY;					// raw current value
		bool xSet = false;			// if x-axis change has been raised by SDL
		bool ySet = false;			// if y-axis change has been raised by SDL
		// individual 
		float lastX;
		float lastY;
		bool xChanged = false;		// if x-axis has logically changed
		bool yChanged = false;		// if y-axis has logically changed
		// axis
		glm::vec2 lastAxis;
		bool axisChanged = false;

	// functions
	public:
		void Update();

		void SetX(float x)
		{
			xSet = true;
			rawX = x;
		}

		void SetY(float y)
		{
			ySet = true;
			rawY = y;
		}

		bool HasXChanged() const
		{
			return xChanged;
		}

		bool HasYChanged() const 
		{
			return yChanged;
		}

		bool HasAxisChanged() const
		{
			return axisChanged;
		}

		float GetX() const
		{
			return lastX;
		}

		float GetY() const
		{
			return lastY;
		}

		glm::vec2 GetAxis() const
		{
			return lastAxis;
		}
	};

// variables 
private:
	std::array< std::array<Axis2DInput, 3> , MAX_PLAYERS> playerAxes;
	Axis2DInput debugPlayerAxis;	// special axis for keyboard input
	CpuProfiler profiler;
	// hot pile of trash
	int dkUp = 0;
	int dkDown = 0;
	int dkLeft = 0;
	int dkRight = 0;
	int dkUpLast = dkUp;
	int dkDownLast = dkDown;
	int dkLeftLast = dkLeft;
	int dkRightLast = dkRight;

// functions
public:
	InputSystem();
	~InputSystem();;

	virtual void Update(float dt) override;

private:
	void NotifyAxis(Axis2DInput& axis, Axis which, int player);
};


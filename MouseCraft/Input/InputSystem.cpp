#include "InputSystem.h"
#include "../Core/OmegaEngine.h"

#pragma region Class::Axis2DInput

void InputSystem::Axis2DInput::Update()
{
	xChanged = false;
	yChanged = false;
	axisChanged = false;

	// parse indiv - x
	if (xSet)
	{
		if (lastX == 0)
		{
			// dead, check if alive 
			if (abs(rawX) > deadzone)
			{
				xChanged = true;
				lastX = rawX;
			}
			// else ignore 
		}
		else
		{
			// active, check if died
			if (abs(rawX) < deadzone)
			{
				lastX = 0;
			}
			else
			{
				lastX = rawX;
			}
			xChanged = true;
		}
	}

	// parase indiv - y
	if (ySet)
	{
		if (lastY == 0)
		{
			// dead, check if alive 
			if (abs(rawY) > deadzone)
			{
				yChanged = true;
				lastY = rawY;
			}
			// else ignore 
		}
		else
		{
			// active, check if died
			if (abs(rawY) < deadzone)
			{
				lastY = 0;
			}
			else
			{
				lastY = rawY;
			}
			yChanged = true;
		}
	}

	// parse axis 
	// note: possible for x and y not to change but for axis to change. (x=0.9, y=0.9, axis>1)
	if (xSet || ySet)
	{
		auto length = sqrt(rawX*rawX + rawY*rawY);
		if (lastAxis == glm::vec2())
		{
			// dead, check if alive
			if (length > deadzone)
			{
				axisChanged = true;
				lastAxis = glm::vec2(rawX, rawY);
			}
		}
		else
		{
			// active, check if died, else normal input.
			lastAxis = (length < deadzone) ? glm::vec2() : glm::vec2(rawX, rawY);
			axisChanged = true;
		}
	}
	
	// cleanup
	xSet = false;
	ySet = false;
}

#pragma endregion

#pragma region Class::InputSystem

InputSystem::InputSystem()
{
	SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK);

	// display some input info
	auto numControllers = SDL_NumJoysticks();
	if (numControllers <= 0)
	{
		std::cout << "WARNING: There are no controllers plugged in!" << std::endl;
	}
	else
	{
		std::cout << "Detected " << numControllers << " controllers." << std::endl;
		for (int i = 0; i < numControllers && i < MAX_PLAYERS; ++i)
		{
			std::cout << "Controller[" << i << "]: " << SDL_GameControllerNameForIndex(i) << std::endl;
			SDL_GameControllerOpen(i);
		}

		if (numControllers > MAX_PLAYERS)
		{
			// Why do you have over 4 controllers? Unsure if this will have performance impact.
			std::cout << "WARNING: Up to 4 controllers can be opened." << std::endl;
		}
	}

	profiler.InitializeTimers(1);
	profiler.LogOutput("Input.log");
}

InputSystem::~InputSystem()
{
}

void InputSystem::Update(float dt)
{
	profiler.StartTimer(0);

	// Note: hot pile of trash for handle SDL2 hot pile of keyboard input trash.
	dkUpLast = dkUp;
	dkDownLast = dkDown;
	dkLeftLast = dkLeft;
	dkRightLast = dkRight;

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			OmegaEngine::Instance().Stop();
		}
		else if (e.type == SDL_CONTROLLERAXISMOTION)
		{
			int player = e.caxis.which;
			float value = (float)e.caxis.value / (float)INT16_MAX;

			if (player >= MAX_PLAYERS)
				continue;

			switch (e.caxis.axis)
			{
			case SDL_CONTROLLER_AXIS_LEFTX:
				playerAxes[player][0].SetX(value);
				break;
			case SDL_CONTROLLER_AXIS_LEFTY:
				playerAxes[player][0].SetY(value);
				break;
			case SDL_CONTROLLER_AXIS_RIGHTX:
				playerAxes[player][1].SetX(value);
				break;
			case SDL_CONTROLLER_AXIS_RIGHTY:
				playerAxes[player][1].SetY(value);
				break;
			case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
				playerAxes[player][2].SetX(value);
				break;
			case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
				playerAxes[player][2].SetY(value);
				break;
			default:
				continue;
			}
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN || e.type == SDL_CONTROLLERBUTTONUP)
		{
			int player = e.cbutton.which;
			bool isDown = e.cbutton.state == SDL_PRESSED;
			Button b;

			switch (e.cbutton.button)
			{
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				b = Button::PRIMARY;
				break;
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
				b = Button::SECONDARY;
				break;
			case SDL_CONTROLLER_BUTTON_A:
				b = Button::SOUTH;
				break;
			case SDL_CONTROLLER_BUTTON_B:
				b = Button::EAST;
				break;
			case SDL_CONTROLLER_BUTTON_X:
				b = Button::WEST;
				break;
			case SDL_CONTROLLER_BUTTON_Y:
				b = Button::NORTH;
				break;
            case SDL_CONTROLLER_BUTTON_GUIDE:
                b = Button::OPTION;
                break;
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
				b = Button::UP;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				b = Button::DOWN;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
				b = Button::LEFT;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				b = Button::RIGHT;
				break;
			default:
				continue;
			}

			// notify
			EventManager::Notify(EventName::INPUT_BUTTON,
				new TypeParam<ButtonEvent>(ButtonEvent{ player, b, isDown }));
		}
		else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
		{
			// std::cout << "KEY: " << (int) e.key.keysym.sym << std::endl;
			
			// handling both states here to minimize LINES
			int player = DEBUG_PLAYER;
			bool isDown = e.type == SDL_KEYDOWN;
			float diff;

			switch (e.key.keysym.sym)
			{
			case SDLK_w:
				dkUp = (isDown) ? -1 : 0;
				break;
			case SDLK_a:
				dkLeft = (isDown) ? -1 : 0;
				break;
			case SDLK_s:
				dkDown = (isDown) ? 1 : 0;
				break;
			case SDLK_d:
				dkRight = (isDown) ? 1 : 0;
				break;
			case SDLK_j:
				EventManager::Notify(EventName::INPUT_BUTTON,
					new TypeParam<ButtonEvent>(ButtonEvent{ player, Button::PRIMARY, isDown }));
				break;
			case SDLK_k:
				EventManager::Notify(EventName::INPUT_BUTTON,
					new TypeParam<ButtonEvent>(ButtonEvent{ player, Button::SECONDARY, isDown }));
				break;
			case SDLK_l:
				EventManager::Notify(EventName::INPUT_BUTTON,
					new TypeParam<ButtonEvent>(ButtonEvent{ player, Button::AUX1, isDown }));
				break;
			case SDLK_SEMICOLON:
				EventManager::Notify(EventName::INPUT_BUTTON,
					new TypeParam<ButtonEvent>(ButtonEvent{ player, Button::AUX2, isDown }));
				break;
            case SDLK_RETURN:
                EventManager::Notify(EventName::INPUT_BUTTON,
					new TypeParam<ButtonEvent>(ButtonEvent{ player, Button::OPTION, isDown }));
                break;
            }

			EventManager::Notify(EventName::INPUT_KEY,
				new TypeParam<KeyEvent>(KeyEvent{ e.key.keysym.sym, isDown }));
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
		{
			bool isDown = e.button.state == SDL_PRESSED;
			bool isRight;
			glm::ivec2 pos;

			if (e.button.button == SDL_BUTTON_LEFT)
			{
				isRight = false;
			}
			else if (e.button.button == SDL_BUTTON_RIGHT)
			{
				isRight = true;
			}
			else
			{
				// some other button - ignore. 
				continue;
			}

			SDL_GetMouseState(&pos.x, &pos.y);

			// notify 
			EventManager::Notify(EventName::INPUT_MOUSE_CLICK,
				new TypeParam<MouseButtonEvent>(MouseButtonEvent{ pos, isRight, isDown }));
		}
		else if (e.type == SDL_MOUSEMOTION)
		{
			EventManager::Notify(EventName::INPUT_MOUSE_MOVE,
				new TypeParam<glm::ivec2>(glm::ivec2(e.motion.x, e.motion.y)));
		}
	} // end while 


	// notify joystick movement 
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		Axis2DInput& leftStick = playerAxes[player][0];
		Axis2DInput& rightStick = playerAxes[player][1];
		Axis2DInput& triggers = playerAxes[player][2];

		// update 
		leftStick.Update();
		rightStick.Update();
		triggers.Update();

		// notify 
		NotifyAxis(leftStick, Axis::STICK_LEFT, player);
		NotifyAxis(rightStick, Axis::STICK_RIGHT, player);
		NotifyAxis(triggers, Axis::TRIGGER, player);
	}

	// notify debug keyboard player 
	// note: we update every frame because SDL2 is stupid and key_down/up event/pressed/release 
	// is already called every frame on hold (instead on change).
	bool dkUpdated = (dkLeft != dkLeftLast) || (dkRight != dkRightLast) || (dkUp != dkUpLast) || (dkDown != dkDownLast);
	if (dkUpdated)
	{
		debugPlayerAxis.SetX(dkLeft + dkRight);
		debugPlayerAxis.SetY(dkUp + dkDown);
	}
	debugPlayerAxis.Update();
	NotifyAxis(debugPlayerAxis, Axis::STICK_LEFT, DEBUG_PLAYER);

	profiler.StopTimer(0);
	profiler.FrameFinish();
}

void InputSystem::NotifyAxis(Axis2DInput& axis, Axis which, int player)
{
	if (axis.HasAxisChanged())
	{
		EventManager::Notify(EventName::INPUT_AXIS_2D,
			new TypeParam<Axis2DEvent>(Axis2DEvent{ player, which, axis.GetAxis() }));
	}
	if (axis.HasXChanged())
	{
		EventManager::Notify(EventName::INPUT_AXIS,
			new TypeParam<AxisEvent>(AxisEvent{ player, static_cast<Axis>(which + 1), axis.GetX() }));
	}
	if (axis.HasYChanged())
	{
		EventManager::Notify(EventName::INPUT_AXIS,
			new TypeParam<AxisEvent>(AxisEvent{ player, static_cast<Axis>(which + 2), axis.GetY() }));
	}
}

#pragma endregion
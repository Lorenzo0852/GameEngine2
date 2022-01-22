#pragma once
#include <string>
#include <EventBus/Event.h>

class InputEvent : public Event
{
public:
	/// <summary>
	/// Name of the input that just happened.
	/// </summary>
	enum Action
	{
		QUIT,
		MINIMIZE,
		FORWARD,
		BACKWARDS,
		LEFT,
		RIGHT,
		JUMP,
		LEFT_ROTATION,
		RIGHT_ROTATION,
		UPWARD_ROTATION,
		DOWNWARD_ROTATION,
		FIRE1,
		FIRE2
	};

	enum Action action;
	/// <summary>
	/// Normalized value of the input, useful for analog controllers like joysticks. Buttons will usually only be 1/0.
	/// </summary>
	float value;

	InputEvent(Action action, float value) : action(action), value(value) {}
};
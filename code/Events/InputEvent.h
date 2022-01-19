#pragma once
#include <string>
#include "../EventBus/Event.h"

class InputEvent : public Event
{
public:
	/// <summary>
	/// Name of the input that just happened.
	/// </summary>
	std::string action;
	/// <summary>
	/// Value of the input, useful for analog controllers like joysticks. Buttons will usually only be 1/0.
	/// </summary>
	float value;

	InputEvent(std::string action, float value) : action(action), value(value) {}
};
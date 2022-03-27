#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <EventBus/Event.h>
#include <ECS/ECS.h>
#include <b2_fixture.h>

namespace engine
{
	class OnTriggerEntryEvent : public Event
	{
	public:
		b2Fixture * entityA;
		b2Fixture * entityB;

		OnTriggerEntryEvent(b2Fixture * entityA, b2Fixture* entityB) : entityA(entityA), entityB(entityB) {}
	};
}
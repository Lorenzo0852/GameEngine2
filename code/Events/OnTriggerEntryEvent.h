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
		b2Fixture * triggerFixture;
		b2Fixture * otherFixture;

		OnTriggerEntryEvent(b2Fixture * entityA, b2Fixture* entityB) : triggerFixture(entityA), otherFixture(entityB) {}
	};
}
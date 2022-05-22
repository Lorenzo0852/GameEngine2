#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <EventBus/Event.h>
#include <ECS/ECS.h>
#include <btBulletDynamicsCommon.h>

namespace engine
{
	class OnCollisionEnter3DEvent : public Event
	{
	public:
		const btCollisionObject* collisionObj;
		const btCollisionObject* otherObj;

		OnCollisionEnter3DEvent(const btCollisionObject* entityA, const btCollisionObject* entityB) : collisionObj(entityA), otherObj(entityB) {}
	};
}
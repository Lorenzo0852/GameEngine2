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
		btRigidBody* collisionRigidbody;
		btRigidBody* otherRigidbody;

		OnCollisionEnter3DEvent(btRigidBody* entityA, btRigidBody* entityB) : collisionRigidbody(entityA), otherRigidbody(entityB) {}
	};
}
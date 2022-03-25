#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/RigidbodyComponent.h>
#include <Components/BoxColliderComponent.h>
#include <Components/CircleColliderComponent.h>

#include <b2_world.h>
#include <spdlog/spdlog.h>

#include <assert.h>

namespace engine
{
	class PhysicsSystem : public System
	{
	private:
		float timeStep;
		int32 velocityIterations, positionIterations;

	public:

		glm::vec2 gravity;
		b2World* world;

		/// Take a time step. This performs collision detection, integration,
		/// and constraint solution.
		/// @param timeStep the amount of time to simulate, this should not vary.
		/// @param velocityIterations for the velocity constraint solver.
		/// @param positionIterations for the position constraint solver.
		PhysicsSystem(
			glm::vec2 gravity = glm::vec2(0.0f, -9.8f),
			float timeStep = 1.f / 60.0f,
			int32 velocityIterations = 8,
			int32 positionIterations = 2)
		{
			RequireComponent<TransformComponent>();
			RequireComponent<RigidbodyComponent>();

			this->gravity = gravity;
			world = new b2World({ gravity.x, gravity.y });

			this->timeStep = timeStep;
			this->velocityIterations = velocityIterations;
			this->positionIterations = positionIterations;
		}

		bool Initialize()
		{
			b2BodyDef bodyDef;

			for (Entity entity : GetSystemEntities())
			{
				TransformComponent& transform = entity.GetComponent<TransformComponent>();
				RigidbodyComponent & rb = entity.GetComponent<RigidbodyComponent>();

				bodyDef.type			= static_cast<b2BodyType>(rb.initialParameters.type);
				bodyDef.position		=	{ transform.position.x + rb.initialParameters.positionOffset.x,
												transform.position.y + rb.initialParameters.positionOffset.y };
				bodyDef.angle			=	transform.rotation.z;
				bodyDef.allowSleep		=	rb.initialParameters.allowSleep;
				bodyDef.angularDamping	=	rb.initialParameters.angularDamping;
				bodyDef.angularVelocity =	rb.initialParameters.angularVelocity;
				bodyDef.awake			=	rb.initialParameters.awake;
				bodyDef.bullet			=	rb.initialParameters.bullet;
				bodyDef.enabled			=	rb.initialParameters.enabled;
				bodyDef.fixedRotation	=	rb.initialParameters.fixedRotation;
				bodyDef.gravityScale	=	rb.initialParameters.gravityScale;
				bodyDef.linearDamping	=	rb.initialParameters.linearDamping;
				bodyDef.linearVelocity	= { rb.initialParameters.linearVelocity.x , rb.initialParameters.linearVelocity.y };
				
				rb.body = world->CreateBody(&bodyDef);

				if (entity.HasComponent<BoxColliderComponent>())
				{
					BoxColliderComponent& bcc = entity.GetComponent<BoxColliderComponent>();
					b2PolygonShape& box = bcc.shape;
					float& density = bcc.density;
					rb.body->CreateFixture(&box, density);
				}
				else if (entity.HasComponent<CircleColliderComponent>())
				{
					CircleColliderComponent& ccc = entity.GetComponent<CircleColliderComponent>();
					b2CircleShape& circle = ccc.shape;
					float& density = ccc.density;
					rb.body->CreateFixture(&circle, density);
				}
				else
				{
					spdlog::error("Rigidbodies can't function without a physics shape! Attach a collider to all physics-affected entities.");
				}
			}
			return true;
		}

		void Run(float deltaTime)
		{
			world->Step(timeStep, velocityIterations, positionIterations);
		}
	};
}
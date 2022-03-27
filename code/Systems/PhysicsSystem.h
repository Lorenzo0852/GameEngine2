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
#include <EventBus/EventBus.h>
#include <Events/OnTriggerEntryEvent.h>

#include <assert.h>

namespace engine
{
	class PhysicsSystem : public System, b2ContactListener
	{
	private:
		float timeStep;
		int32 velocityIterations, positionIterations;

	public:
		std::shared_ptr<EventBus> eventBus;
		glm::vec2 gravity;
		b2World* world;

		/// Take a time step. This performs collision detection, integration,
		/// and constraint solution.
		/// @param timeStep the amount of time to simulate, this should not vary.
		/// @param velocityIterations for the velocity constraint solver.
		/// @param positionIterations for the position constraint solver.
		PhysicsSystem(
			std::shared_ptr<EventBus> eventBus,
			glm::vec2 gravity = glm::vec2(0.0f, -9.8f),
			float timeStep = 1.f / 60.0f,
			int32 velocityIterations = 8,
			int32 positionIterations = 2)
		{
			RequireComponent<TransformComponent>();
			RequireComponent<RigidbodyComponent>();

			this->eventBus = eventBus;
			this->gravity = gravity;
			world = new b2World({ gravity.x, gravity.y });
			world->SetContactListener(this);
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
					b2FixtureDef fd;
					BoxColliderComponent& bcc = entity.GetComponent<BoxColliderComponent>();
					b2PolygonShape& box = bcc.shape;
					float& density = bcc.density;
					float& friction = bcc.friction;
					fd.shape = &box;
					fd.density = density;
					fd.friction = friction;
					fd.isSensor = bcc.isTrigger;

					rb.body->CreateFixture(&fd);
				}
				else if (entity.HasComponent<CircleColliderComponent>())
				{
					b2FixtureDef fd;
					CircleColliderComponent& ccc = entity.GetComponent<CircleColliderComponent>();
					b2CircleShape& circle = ccc.shape;
					float& density = ccc.density;
					float& friction = ccc.friction;
					fd.shape = &circle;
					fd.density = density;
					fd.friction = friction;
					fd.isSensor = ccc.isTrigger;
					rb.body->CreateFixture(&fd);
				}
				else
				{
					spdlog::error("Rigidbodies can't function without a physics shape! Attach a collider to all physics-affected entities.");
					return false;
				}
			}
			return true;
		}

		void Run(float deltaTime)
		{
			world->Step(timeStep, velocityIterations, positionIterations);
		}

		/// <summary>
		/// Welds entity A to body B.
		/// </summary>
		/// <param name="entityA">Entity to weld</param>
		/// <param name="entityB">Entity to be welded to</param>
		void Weld(Entity* entityA, Entity* entityB)
		{
			b2Body * bodyA = entityA->GetComponent<RigidbodyComponent>().body;
			b2Body * bodyB = entityB->GetComponent<RigidbodyComponent>().body;

			b2WeldJointDef jointDef;
			jointDef.Initialize(bodyA, bodyB, bodyB->GetPosition());
			world->CreateJoint(&jointDef);
		}

		/// <summary>
		/// Welds entity A to body B with an adjustment in relativePosition.
		/// </summary>
		/// <param name="entityA">Entity to weld</param>
		/// <param name="entityB">Entity to be welded to</param>
		void Weld(Entity* entityA, Entity* entityB, glm::vec2 relativePosition)
		{
			b2Body* bodyA = entityA->GetComponent<RigidbodyComponent>().body;
			b2Body* bodyB = entityB->GetComponent<RigidbodyComponent>().body;

			b2WeldJointDef jointDef;
			jointDef.Initialize(bodyA, bodyB, {relativePosition.x, relativePosition.y});
			world->CreateJoint(&jointDef);
		}


		class Motor
		{
		private:
			b2RevoluteJoint* revoluteJoint;

		public:

			/// Set the motor speed in radians per second.
			void EnableMotor(bool flag)
			{
				revoluteJoint->EnableMotor(flag);
			}

			/// Set the motor speed in radians per second.
			void SetMotorSpeed(float speed)
			{
				revoluteJoint->SetMotorSpeed(speed);
			}

			/// Get the motor speed in radians per second.
			float GetMotorSpeed() const
			{
				return revoluteJoint->GetMotorSpeed();
			}

			Motor(b2RevoluteJoint* b2rj) : revoluteJoint(b2rj) {}

			Motor() = default;
		};

		Motor Motorize(Entity* entityA,
			Entity* entityB,
			float maxMotorTorque,
			glm::vec2 relativePosition = glm::vec2(0),
			float lowerAngleLimit = 0,
			float upperAngleLimit = 2 * 3.614159)
		{
			b2RevoluteJointDef revJoint;
			revJoint.enableMotor = true;
			revJoint.maxMotorTorque = maxMotorTorque;
			revJoint.bodyB = entityA->GetComponent<RigidbodyComponent>().body;
			revJoint.bodyA = entityB->GetComponent<RigidbodyComponent>().body;
			revJoint.localAnchorA = { relativePosition.x, relativePosition.y };
			revJoint.localAnchorB = { relativePosition.x, -relativePosition.y };
			revJoint.enableLimit = true;
			revJoint.lowerAngle = lowerAngleLimit;
			revJoint.upperAngle = upperAngleLimit;

			return Motor((b2RevoluteJoint*)world->CreateJoint(&revJoint));
		}

		class Wheel
		{
		private:
			b2WheelJoint* wheelJoint;
		public:

			/// Set the motor speed in radians per second.
			void EnableMotor(bool flag)
			{
				wheelJoint->EnableMotor(flag);
			}

			/// Set the motor speed in radians per second.
			void SetMotorSpeed(float speed)
			{
				wheelJoint->SetMotorSpeed(speed);
			}

			/// Get the motor speed in radians per second.
			float GetMotorSpeed() const
			{
				return wheelJoint->GetMotorSpeed();
			}

			Wheel(b2WheelJoint* b2wj) : wheelJoint(b2wj) {}

			Wheel() = default;
		};

		Wheel MotorizeAsWheel(Entity* entityA, Entity* entityB, float maxMotorTorque, glm::vec2 axis = glm::vec2(0))
		{
			b2WheelJointDef jd;
			jd.Initialize(entityB->GetComponent<RigidbodyComponent>().body,
				entityA->GetComponent<RigidbodyComponent>().body,
				entityA->GetComponent<RigidbodyComponent>().body->GetPosition(),
				{ axis.x, axis.y });

			float mass1 = entityB->GetComponent<RigidbodyComponent>().body->GetMass();
			float mass2 = entityA->GetComponent<RigidbodyComponent>().body->GetMass();

			float hertz = 4.0f;
			float dampingRatio = 0.7f;
			float omega = 2.0f * b2_pi * hertz;

			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = maxMotorTorque;
			jd.enableMotor = true;
			jd.stiffness = mass1 * omega * omega;
			jd.damping = 2.0f * mass1 * dampingRatio * omega;
			jd.lowerTranslation = -0.25f;
			jd.upperTranslation = 0.25f;
			jd.enableLimit = true;

			return Wheel((b2WheelJoint*)world->CreateJoint(&jd));
		}

		class PrismaticJoint
		{

		private:
			b2PrismaticJoint* prismaticJoint;
		
		public:
			void SetLimits(float lower, float upper)
			{
				prismaticJoint->SetLimits(lower, upper);
			}

			void EnableMotor(bool flag)
			{
				prismaticJoint->EnableMotor(flag);
			}

			/// Set the motor speed in radians per second.
			void SetMotorSpeed(float speed)
			{
				prismaticJoint->SetMotorSpeed(speed);
			}

			/// Get the motor speed in radians per second.
			float GetMotorSpeed() const
			{
				return prismaticJoint->GetMotorSpeed();
			}

			PrismaticJoint(b2PrismaticJoint * b2pj) : prismaticJoint(b2pj) {}

			PrismaticJoint() = default;
		};

		PrismaticJoint MotorizedPrismaticJoint(
			Entity* entityA,
			Entity* entityB,
			float maxMotorForce,
			float lowerTranslation,
			float upperTranslation,
			glm::vec2 axis = glm::vec2(0))
		{
			b2PrismaticJointDef pjd;

			pjd.Initialize(entityB->GetComponent<RigidbodyComponent>().body,
				entityA->GetComponent<RigidbodyComponent>().body,
				entityA->GetComponent<RigidbodyComponent>().body->GetPosition(),
				{ axis.x, axis.y });

			pjd.motorSpeed = 0;
			pjd.maxMotorForce = maxMotorForce;
			pjd.enableMotor = true;
			pjd.lowerTranslation = lowerTranslation;
			pjd.upperTranslation = upperTranslation;
			pjd.enableLimit = true;

			return PrismaticJoint((b2PrismaticJoint*)world->CreateJoint(&pjd));
		}

		void BeginContact(b2Contact* contact) override
		{
			b2Fixture* fixtureA = contact->GetFixtureA();
			b2Fixture* fixtureB = contact->GetFixtureB();

			//Just in case someone tries to collide two trigger/sensor objects (which shouldn't invoke the event)
			if (fixtureA->IsSensor() ^ fixtureB->IsSensor())
			{
				eventBus->FireEvent<OnTriggerEntryEvent>(fixtureA, fixtureB);
			}
		}

		void EndContact(b2Contact* contact) override {

		}

	};
}
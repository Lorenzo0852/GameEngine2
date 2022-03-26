#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <glm/glm.hpp>
#include <box2d.h>

namespace engine
{
	enum BodyType
	{
		staticBody = 0,
		kinematicBody,
		dynamicBody
	};

	/// <summary>
	/// Holds the physics object. Needs a collider component to work properly!
	/// (There can't be any physics without a shape/object to apply them to)
	/// </summary>
	struct RigidbodyComponent
	{
		struct InitialParameters
		{
		public:
			/// The body type: static, kinematic, or dynamic.
			/// Note: if a dynamic body would have zero mass, the mass is set to one.
			BodyType type = dynamicBody;

			/// The world position offset of the body relative to its entitie's location
			glm::vec2 positionOffset = { 0.f, 0.f };

			/// The linear velocity of the body's origin in world co-ordinates.
			glm::vec2 linearVelocity = { 0.f, 0.f };

			/// The angular velocity of the body.
			float angularVelocity = 0.f;

			/// Linear damping is use to reduce the linear velocity. The damping parameter
			/// can be larger than 1.0f but the damping effect becomes sensitive to the
			/// time step when the damping parameter is large.
			/// Units are 1/time
			float linearDamping = 0.f;

			/// Angular damping is use to reduce the angular velocity. The damping parameter
			/// can be larger than 1.0f but the damping effect becomes sensitive to the
			/// time step when the damping parameter is large.
			/// Units are 1/time
			float angularDamping = 0.f;

			/// Set this flag to false if this body should never fall asleep. Note that
			/// this increases CPU usage.
			bool allowSleep = true;

			/// Is this body initially awake or sleeping?
			bool awake = true;

			/// Should this body be prevented from rotating? Useful for characters.
			bool fixedRotation = false;

			/// Is this a fast moving body that should be prevented from tunneling through
			/// other moving bodies? Note that all bodies are prevented from tunneling through
			/// kinematic and static bodies. This setting is only considered on dynamic bodies.
			/// @warning You should use this flag sparingly since it increases processing time.
			bool bullet = false;

			/// Does this body start out enabled?
			bool enabled = true;

			/// Scale the gravity applied to this body.
			float gravityScale = 1.0f;
		};

	public:
		InitialParameters initialParameters;
		/// <summary>
		/// Will only exist after initializing the physics system!
		/// </summary>
		b2Body* body;

		RigidbodyComponent(BodyType type = dynamicBody) {
			this->initialParameters.type = type;
			this->body = nullptr;
		}
	};

}
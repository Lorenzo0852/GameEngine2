#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/Rigidbody3DComponent.h>

namespace engine
{
	class MovementSystem : public System
	{
	public:
		MovementSystem()
		{
			// We specify the components that our system is interested in.
			RequireComponent<TransformComponent>();
			RequireComponent<Rigidbody3DComponent>();
		}

		static std::shared_ptr< System > CreateInstance()
		{
			return std::make_shared<MovementSystem>();
		}

		void Run(float deltaTime)
		{
			for (auto entity : GetSystemEntities())
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				const auto& rigidbody = entity.GetComponent<Rigidbody3DComponent>();

				transform.position.x += static_cast<float>(rigidbody.velocity.x * deltaTime);
				transform.position.y += static_cast<float>(rigidbody.velocity.y * deltaTime);
				transform.position.z += static_cast<float>(rigidbody.velocity.z * deltaTime);
			}
		}
	};
}
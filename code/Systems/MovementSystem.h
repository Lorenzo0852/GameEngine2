#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		// We specify the components that our system is interested in.
		RequireComponent<TransformComponent>();
		RequireComponent<RigidbodyComponent>();
	}

	void Update(double deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

			transform.position.x += static_cast<float>(rigidbody.velocity.x * deltaTime);
			transform.position.y += static_cast<float>(rigidbody.velocity.y * deltaTime);
			transform.position.z += static_cast<float>(rigidbody.velocity.z * deltaTime);
		}
	}

	void Run(float deltaTime)
	{

	}
};
#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/Node3DComponent.h"
#include "../gltk/Node.hpp"

class Movement3DSystem : public System
{
public:
	Movement3DSystem()
	{
		// We specify the components that our system is interested in.
		RequireComponent<TransformComponent>();
		RequireComponent<RigidbodyComponent>();
		RequireComponent<Node3DComponent>();
	}

	void Run(float deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
			node->translate(glm::vec3(rigidbody.velocity.x * deltaTime, rigidbody.velocity.y * deltaTime, rigidbody.velocity.z * deltaTime));

			transform.position.x += static_cast<float>(rigidbody.velocity.x * deltaTime);
			transform.position.y += static_cast<float>(rigidbody.velocity.y * deltaTime);
			transform.position.z += static_cast<float>(rigidbody.velocity.z * deltaTime);

			node->rotate_around_x(rigidbody.angularVelocity.x * deltaTime);
			node->rotate_around_y(rigidbody.angularVelocity.y * deltaTime);
			node->rotate_around_z(rigidbody.angularVelocity.z * deltaTime);
		}
	}
};
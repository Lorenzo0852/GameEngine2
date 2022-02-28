#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/RigidbodyComponent.h>
#include <Components/Node3DComponent.h>
#include <gltk/Node.hpp>

namespace engine
{
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

		static std::shared_ptr< System > CreateInstance()
		{
			return std::make_shared<Movement3DSystem>();
		}

		void Run(float deltaTime)
		{
			for (auto& entity : GetSystemEntities())
			{
				std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
				auto& transform = entity.GetComponent<TransformComponent>();
				const auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

				node->translate(glm::vec3(rigidbody.velocity.x * deltaTime, rigidbody.velocity.y * deltaTime, rigidbody.velocity.z * deltaTime));

				transform.rotation.x += static_cast<float>(rigidbody.angularVelocity.x * deltaTime);
				transform.rotation.y += static_cast<float>(rigidbody.angularVelocity.y * deltaTime);
				transform.rotation.z += static_cast<float>(rigidbody.angularVelocity.z * deltaTime);

				node->rotate_around_x(rigidbody.angularVelocity.x * deltaTime);
				node->rotate_around_y(rigidbody.angularVelocity.y * deltaTime);
				node->rotate_around_z(rigidbody.angularVelocity.z * deltaTime);

				glm::vec3 translation = glt::extract_translation(node->get_transformation());
				transform.position = glm::vec3(translation.x, translation.y, translation.z);
			}
		}

		void MoveToPosition(Entity& entity, glm::vec3 position)
		{
			std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
			auto& transform = entity.GetComponent<TransformComponent>();

			node->set_transformation(glm::mat4(1));
			node->translate(position);
			node->rotate_around_x(transform.rotation.x);
			node->rotate_around_y(transform.rotation.y);
			node->rotate_around_z(transform.rotation.z);
			node->scale(transform.scale.x, transform.scale.y, transform.scale.z);
		}

		void ResetTransform(Entity& entity)
		{
			std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
			auto& transform = entity.GetComponent<TransformComponent>();

			node->set_transformation(glm::mat4(1));
			node->translate(transform.initialPosition);
			node->rotate_around_x(transform.initialRotation.x);
			node->rotate_around_y(transform.initialRotation.y);
			node->rotate_around_z(transform.initialRotation.z);
			node->scale(transform.initialScale.x, transform.initialScale.y, transform.initialScale.z);
		}

		glm::vec3 MoveTowards(Entity& entity, glm::vec3 destination)
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			glm::vec3 direction = destination - transform.position;
			float length = glm::length(direction);
			glm::vec3 normalized = glm::vec3(direction.x / length, direction.y / length, direction.z / length);
			return normalized;
		}

		float Distance(glm::vec3 from, glm::vec3 to)
		{
			return glm::distance(from, to);
		}
	};
}
#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/RigidbodyComponent.h>
#include <Components/Rigidbody3DComponent.h>

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
			//RequireComponent<RigidbodyComponent>();
			RequireComponent<Node3DComponent>();
		}

		void Run(float deltaTime)
		{
			for (auto& entity : GetSystemEntities())
			{
				//2D
				if (entity.HasComponent<RigidbodyComponent>())
				{
					std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
					auto& transform = entity.GetComponent<TransformComponent>();
					const auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

					b2Vec2 position = rigidbody.body->GetPosition();
					float angle = rigidbody.body->GetAngle();
					transform.SetTransformation(
						{ position.x, position.y, 0 },
						{ 0, 0, angle },
						{ transform.scale.x,transform.scale.y,transform.scale.z });

					node->set_transformation(transform.GetTransformation());
				}
				
				//3D
				if (entity.HasComponent<Rigidbody3DComponent>())
				{
					std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
					auto& transform = entity.GetComponent<TransformComponent>();
					const auto& rigidbody3d = entity.GetComponent<Rigidbody3DComponent>();

					btVector3 position = rigidbody3d.rigidbody->getCenterOfMassPosition();

					transform.SetTransformation(
						{position.x(), position.y(), position.z()},
						{0,0,0},
						{transform.scale.x, transform.scale.y, transform.scale.z});

					spdlog::info(std::to_string(position.y()));

					node->set_transformation(transform.GetTransformation());
				}

			}
		}
	};
}
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
				
				node->set_transformation(transform.GetTransformation());
			}
		}
	};
}
#pragma once
#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/Node3DComponent.h>
#include <gltk/Node.hpp>

namespace engine
{
	/// <summary>
	/// Updates each entities' transform when they are not tied to physics-based movement.
	/// </summary>
	class PositionUpdater : public System
	{
	public:
		PositionUpdater()
		{
			// We specify the components that our system is interested in.
			RequireComponent<TransformComponent>();
		}

		void Run(float deltaTime)
		{
			for (auto& entity : GetSystemEntities())
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
				node->set_transformation(transform.GetTransformation());
			}
		}
	};
}
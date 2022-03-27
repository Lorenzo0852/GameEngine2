#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/Node3DComponent.h>
#include <Window/Window.h>
#include <gltk/Render_Node.hpp>
#include <spdlog/spdlog.h>

namespace engine
{
	/// <summary>
/// Starts up all existing entities with a transform (ALL).
/// </summary>
	class EntityStartup3DSystem : public System
	{
	public:
		EntityStartup3DSystem()
		{
			// We specify the components that our system is interested in.
			RequireComponent<TransformComponent>();
			RequireComponent<Node3DComponent>();
		}


		static std::shared_ptr< System > CreateInstance()
		{
			return std::make_shared<EntityStartup3DSystem>();
		}


		bool Initialize()
		{
			spdlog::info("Starting up entities' transforms...");

			for (auto entity : GetSystemEntities())
			{
				Node3DComponent openGlComp = entity.GetComponent<Node3DComponent>();
				TransformComponent& transform = entity.GetComponent<TransformComponent>();
				openGlComp.node->set_transformation(transform.GetTransformation());
			}

			return true;
		}

		void Run(float deltaTime)
		{

		}
	};
}
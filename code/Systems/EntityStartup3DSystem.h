#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/Node3DComponent.h"
#include "../Window/Window.h"
#include <Render_Node.hpp>
#include <spdlog/spdlog.h>

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
			std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
			auto& transform = entity.GetComponent<TransformComponent>();
			node->translate(transform.position);
			node->scale(transform.scale.x, transform.scale.y, transform.scale.z);
			node->rotate_around_x(transform.rotation.x);
			node->rotate_around_y(transform.rotation.y);
			node->rotate_around_z(transform.rotation.z);

			if (transform.parent)
			{
				node->set_parent(transform.parent->GetComponent<Node3DComponent>().node.get());
			}
		}
		return true;
	}

	void Run(float deltaTime)
	{

	}
};
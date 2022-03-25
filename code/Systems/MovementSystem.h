#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/RigidbodyComponent.h>

namespace engine
{
	class MovementSystem : public System
	{
	public:
		MovementSystem()
		{
			// We specify the components that our system is interested in.
			RequireComponent<TransformComponent>();
			RequireComponent<RigidbodyComponent>();
		}


		static std::shared_ptr< System > CreateInstance()
		{
			return std::make_shared<MovementSystem>();
		}


		void Run(float deltaTime)
		{

		}
	};
}
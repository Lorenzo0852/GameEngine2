#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <glm/glm.hpp>
#include <ECS/ECS.h>

namespace engine
{
	/// <summary>
/// Components are data-driven structures. Nothing more.
/// All functionality needed in a component is managed in the "System" part of Entity-Component-System
/// </summary>
	struct TransformComponent {

		Entity* parent = NULL;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		glm::vec3 initialPosition;
		glm::vec3 initialRotation;
		glm::vec3 initialScale;

		TransformComponent(
			glm::vec3 position = glm::vec3(0, 0, 0),
			glm::vec3 rotation = glm::vec3(0, 0, 0),
			glm::vec3 scale = glm::vec3(0, 0, 0),
			Entity* parent = NULL) {

			this->position = position;
			this->scale = scale;
			this->rotation = rotation;
			this->parent = parent;
		}

	};
}
#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <glm/glm.hpp>
#include "../ECS/ECS.h"

/// <summary>
/// Components are data-driven structures. Nothing more.
/// All functionality needed in a component is managed in the "System" part of Entity-Component-System
/// </summary>
struct TransformComponent {

	Entity* parent;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	TransformComponent(
		glm::vec3 position = glm::vec3(0,0,0),
		glm::vec3 rotation = glm::vec3(0, 0, 0),
		glm::vec3 scale = glm::vec3(0, 0, 0)) {

		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
	}
};
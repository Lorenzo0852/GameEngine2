#pragma once

#include <glm/glm.hpp>

/// <summary>
/// Components are data-driven structures. Nothing more.
/// All functionality needed in a component is managed in the "System" part of Entity-Component-System
/// </summary>
struct TransformComponent {
	glm::vec3 position;
	glm::vec3 scale;
	double rotation;

	TransformComponent(
		glm::vec3 position = glm::vec3(0,0,0),
		glm::vec3 scale = glm::vec3(0,0,0),
		double rotation = 0.0) {

		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
	}
};
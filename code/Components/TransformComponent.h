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
};
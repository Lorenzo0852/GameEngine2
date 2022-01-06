#pragma once

#include <glm/glm.hpp>

struct RigidbodyComponent 
{
	glm::vec3 velocity;

	RigidbodyComponent(	glm::vec3 velocity = glm::vec3(0.0, 0.0, 0.0)	) {
		this->velocity = velocity;
	}
};

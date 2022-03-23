#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <glm/glm.hpp>
#include <box2d.h>

namespace engine
{
	struct RigidbodyComponent
	{
		glm::vec3 velocity;
		glm::vec3 angularVelocity;

		RigidbodyComponent(glm::vec3 velocity = glm::vec3(0.0, 0.0, 0.0), glm::vec3 angularVelocity = glm::vec3(0.0, 0.0, 0.0)) {
			this->velocity = velocity;
			this->angularVelocity = angularVelocity;
		}
	};

}
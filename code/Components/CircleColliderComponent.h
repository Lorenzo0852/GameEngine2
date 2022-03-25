/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#pragma once
#include <glm/glm.hpp>
#include <box2d.h>

namespace engine
{
	struct CircleColliderComponent
	{
		float radius, density;
		b2CircleShape shape;

		CircleColliderComponent(float radius = 1.0f, float density = 1.0f) {
			this->radius = radius;
			this->density = density;

			shape.m_radius = radius;
		}
	};

}
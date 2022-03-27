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
		float radius, density, friction;
		b2CircleShape shape;
		bool isTrigger;

		CircleColliderComponent(bool isTrigger = false, float radius = 1.0f, float density = 1.0f, float friction = .5f) {
			this->radius = radius;
			this->density = density;
			this->friction = friction;
			this->isTrigger = isTrigger;

			shape.m_radius = radius;
		}
	};

}
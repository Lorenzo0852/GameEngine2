#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <glm/glm.hpp>
#include <box2d.h>

namespace engine
{
	struct BoxColliderComponent
	{
		float dimensions[2];
		float density;
		b2PolygonShape shape;

		BoxColliderComponent(float width = 1.0f, float height = 1.0f, float density = 1.0f) {
			this->dimensions[0] = width;
			this->dimensions[1] = height;
			shape.SetAsBox(dimensions[0], dimensions[1]);
		}
	};

}
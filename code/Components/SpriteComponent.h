#pragma once

#include <glm/glm.hpp>

/// <summary>
/// Components are data-driven structures. Nothing more.
/// All functionality needed in a component is managed in the "System" part of Entity-Component-System
/// </summary>
struct SpriteComponent{
	int width;
	int height;

	SpriteComponent(int width = 0, int height = 0) {

		this->width = width;
		this->height = height;
	}
};
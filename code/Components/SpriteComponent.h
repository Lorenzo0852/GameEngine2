#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <glm/glm.hpp>

/// <summary>
/// Components are data-driven structures. Nothing more.
/// All functionality needed in a component is managed in the "System" part of Entity-Component-System
/// </summary>
struct SpriteComponent{
	std::string assetId;
	int width;
	int height;

	SpriteComponent(std::string assetId = "", int width = 0, int height = 0) {
		this->assetId = assetId;
		this->width = width;
		this->height = height;
	}
};
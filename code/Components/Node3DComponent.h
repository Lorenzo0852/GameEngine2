#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <glm/glm.hpp>
#include <sdl2/SDL.h>
#include <gltk/Cube.hpp>
#include <gltk/Model.hpp>
#include <gltk/Model_Obj.hpp>

namespace engine
{
	/// <summary>
/// Components are data-driven structures. Nothing more.
/// All functionality needed in a component is managed in the "System" part of Entity-Component-System
/// </summary>
	struct Node3DComponent {
		std::string modelId;
		std::shared_ptr< glt::Node  > node;

		Node3DComponent(std::string assetId = "", std::shared_ptr<glt::Node> node = nullptr) {
			this->modelId = assetId;
			this->node = node;
		}
	};
}
#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetManager/AssetManager.h"

class Render3DSystem : public System
{
public:
	Render3DSystem()
	{
		// We specify the components that our system is interested in.
		RequireComponent<TransformComponent>();
	}

	void Render(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetStore)
	{
		for (auto entity : GetSystemEntities())
		{
			const TransformComponent transform = entity.GetComponent<TransformComponent>();
			const SpriteComponent sprite = entity.GetComponent<SpriteComponent>();

			//Section of our original sprite texture, useful for things such as tilemaps.
			SDL_Rect srcRect = sprite.srcRect;
			//Set the destination rectangle with the x,y position to be rendered.
			SDL_Rect dstRect = {
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				static_cast<int>(sprite.width * transform.scale.x),
				static_cast<int>(sprite.height * transform.scale.y)
			};

			SDL_RenderCopyEx(
				renderer,
				assetStore->GetTexture(sprite.assetId),
				&srcRect,
				&dstRect,
				transform.rotation,
				NULL,
				SDL_FLIP_NONE);
		}
	}
};
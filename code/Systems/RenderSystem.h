#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <sdl2/SDL.h>
#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/SpriteComponent.h>
#include <AssetManager/AssetManager.h>

namespace engine
{
	class RenderSystem : public System
	{
	private:
		SDL_Renderer* renderer;
	public:
		RenderSystem()
		{
			// We specify the components that our system is interested in.
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
		}


		//static std::shared_ptr< System > CreateInstance()
		//{
		//	return std::make_shared<RenderSystem>();
		//}


		void Render(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetStore)
		{
			this->renderer = renderer;
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
					transform.rotation.x,
					NULL,
					SDL_FLIP_NONE);
			}
		}

		void Run(float deltaTime)
		{

		}

		~RenderSystem()
		{
			SDL_DestroyRenderer(renderer);
		}
	};
}
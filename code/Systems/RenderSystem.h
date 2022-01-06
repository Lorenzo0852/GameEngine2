#pragma once

#include "../sdl2/SDL.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		// We specify the components that our system is interested in.
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer)
	{
		for (auto entity : GetSystemEntities())
		{
			const TransformComponent transform = entity.GetComponent<TransformComponent>();
			const SpriteComponent sprite = entity.GetComponent<SpriteComponent>();

			SDL_Rect objRect = { static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				sprite.width, sprite.height };

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(renderer, &objRect);
		}
	}
};
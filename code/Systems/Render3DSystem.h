#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <sdl2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/OpenGLNodeComponent.h"
#include "../AssetManager/AssetManager.h"
#include "../Window/Window.h"
#include <spdlog/spdlog.h>

class Render3DSystem : public System
{
public:
	Render3DSystem()
	{
		// We specify the components that our system is interested in.
		RequireComponent<TransformComponent>();
		RequireComponent<OpenGLNodeComponent>();
	}

	void LoadEntities(glt::Render_Node & glRenderer)
	{
		spdlog::info("Adding entities to OpenGL renderer...");
		for (auto entity : GetSystemEntities())
		{
			OpenGLNodeComponent openGlComp = entity.GetComponent<OpenGLNodeComponent>();
			glRenderer.add(openGlComp.modelId, openGlComp.node);
			spdlog::info("Adding" + openGlComp.modelId + " to renderer");
		}
	}

	void Render(glt::Render_Node & glRenderer, Window & window)
	{
		for (auto entity : GetSystemEntities())
		{
			const TransformComponent transform = entity.GetComponent<TransformComponent>();
			const OpenGLNodeComponent model = entity.GetComponent<OpenGLNodeComponent>();

			////Section of our original sprite texture, useful for things such as tilemaps.
			//SDL_Rect srcRect = sprite.srcRect;
			////Set the destination rectangle with the x,y position to be rendered.
			//SDL_Rect dstRect = {
			//	static_cast<int>(transform.position.x),
			//	static_cast<int>(transform.position.y),
			//	static_cast<int>(sprite.width * transform.scale.x),
			//	static_cast<int>(sprite.height * transform.scale.y)
			//};


			/*SDL_RenderCopyEx(
				renderer,
				assetStore->GetTexture(sprite.assetId),
				&srcRect,
				&dstRect,
				transform.rotation,
				NULL,
				SDL_FLIP_NONE);*/
		}

		glClearColor(0.5, 0.5f, 0.5f, 1);
		window.Clear();
		glRenderer.render();
		window.SwapBuffers();
	}
};
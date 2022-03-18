/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/
#pragma once

#include <sdl2/SDL.h>
#include <sdl2/SDL_mixer.h>
#include <ECS/ECS.h>
#include <AssetManager/AssetManager.h>
#include <Window/Window.h>
#include <gltk/Render_Node.hpp>
#include <Kernel/Kernel.h>
#include <EventBus/EventBus.h>
#include <Events/InputEvent.h>

#include <b2_world.h>
#include <b2_body.h>

using namespace engine;
namespace game
{
	class Game : public Task
	{
	private:
		Window* window;
		SDL_Renderer* renderer;
		Kernel* kernel;

		std::unique_ptr<Registry> registry;
		std::unique_ptr<AssetManager> assetManager;
		std::shared_ptr<EventBus> eventBus;

	private:
		Entity player;
		Entity wheel;

		b2Body*	mainBody;
		b2Body* wheelBody;
		b2World* world;

		Mix_Chunk* sound;
		Mix_Chunk* death;

	public:
		Game(Window& window, Kernel& kernel, std::shared_ptr<EventBus> eventBus);
		~Game() = default;

		void SetupScene();
		virtual void Run(float deltaTime);
		void OnInputRegistered(InputEvent& event);
	};
}
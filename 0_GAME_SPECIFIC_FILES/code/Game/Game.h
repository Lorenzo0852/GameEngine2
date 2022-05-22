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
#include <Events/OnTriggerEntryEvent.h>
#include <Events/OnCollisionEnter3DEvent.h>

#include <b2_world.h>
#include <b2_body.h>

#include <Systems/PhysicsSystem.h>
#include <Systems/Physics3DSystem.h>

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
		bool playerHasKey = false;

		Entity test;
		Entity key;

		Entity wheel1, wheel2, wheel3, wheel4;

		Entity cam;

	public:
		Game(Window& window, Kernel& kernel, std::shared_ptr<EventBus> eventBus);
		~Game() = default;

		void SetupScene();
		/* Post-setup stuff */
		bool Initialize();
		virtual void Run(float deltaTime);

		void OnInputRegistered(InputEvent & event);
		void OnCollisionEnter3D(OnCollisionEnter3DEvent & event);
		void OnTriggerEntry(OnTriggerEntryEvent & event);
	};
}
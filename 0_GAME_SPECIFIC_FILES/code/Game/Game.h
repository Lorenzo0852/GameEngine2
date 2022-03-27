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

#include <b2_world.h>
#include <b2_body.h>

#include <Systems/PhysicsSystem.h>

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
		Entity car_base;
		Entity wheel_1;
		Entity wheel_2;
		Entity cam;

		PhysicsSystem::Wheel back_wheel_motor, front_wheel_motor;

		float m_speed;

		b2RevoluteJoint* m_car_container_joint_motor;

	public:
		Game(Window& window, Kernel& kernel, std::shared_ptr<EventBus> eventBus);
		~Game() = default;

		void SetupScene();
		/* Post-initialization stuff */
		bool Initialize();
		virtual void Run(float deltaTime);
		void OnInputRegistered(InputEvent& event);
		void OnTriggerEntry(OnTriggerEntryEvent& event);
	};
}
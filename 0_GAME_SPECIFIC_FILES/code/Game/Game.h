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
		//Bottom and both sides of the container on top of the car.
		Entity car_container[3];
		Entity wheel_1;
		Entity wheel_2;

		Entity platform;
		Entity right_trigger, platform_trigger;
		Entity prismatic;

		Entity container_base, container_left, container_right;

		Entity cam;

		PhysicsSystem::Wheel back_wheel_motor, front_wheel_motor;
		PhysicsSystem::Motor car_container_motor;
		PhysicsSystem::Motor container_motor;
		PhysicsSystem::PrismaticJoint prismaticJoint;

		float m_speed;

	public:
		Game(Window& window, Kernel& kernel, std::shared_ptr<EventBus> eventBus);
		~Game() = default;

		void SetupScene();
		/* Post-setup stuff */
		bool Initialize();
		virtual void Run(float deltaTime);
		void OnInputRegistered(InputEvent& event);
		void OnTriggerEntry(OnTriggerEntryEvent& event);
	};
}
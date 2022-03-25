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
		Entity wheel2;
		Entity wheel;
		Entity cam;

		b2Body*	m_car;
		b2Body* m_wheel1;
		b2Body* m_wheel2;

		b2WheelJoint* m_spring1;
		b2WheelJoint* m_spring2;

		Mix_Chunk* sound;
		Mix_Chunk* death;

		float m_speed;
		b2World* m_world;
		b2RevoluteJoint* m_car_container_joint_motor;

	public:
		Game(Window& window, Kernel& kernel, std::shared_ptr<EventBus> eventBus);
		~Game() = default;

		void SetupScene();
		virtual void Run(float deltaTime);
		void OnInputRegistered(InputEvent& event);
	};
}
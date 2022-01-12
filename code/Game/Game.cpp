/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

//#include <gltk/Cube.hpp>
//#include <gltk/Model.hpp>
//#include <gltk/Model_Obj.hpp>
//#include <gltk/Light.hpp>
//#include <gltk/Render_Node.hpp>
//#include <gltk/Camera.hpp>


#include <iostream>
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include <glm/glm.hpp>
#include <Model.hpp>
#include "Game.h"

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/Render3DSystem.h"




Game::Game()
{
	gameLogger = spdlog::rotating_logger_mt("GameLogger", "logs/GameLogs.txt", max_size, max_files);
	spdlog::set_default_logger(gameLogger);
	isRunning = false;
	registry = std::make_unique<Registry>();
	assetManager = std::make_unique<AssetManager>();
	gameLogger->info("Game constructor called");
}

void Game::Initialize(Window & window)
{
	window.SetWindowedFullscreen();

	this->window = &window;
	//-1 index means "get the default monitor/screen for the renderer".
	//renderer = SDL_CreateRenderer(window.sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC);

	glRenderer.reset(new glt::Render_Node);
	std::shared_ptr< glt::Model  > cube(new glt::Model);
	//std::shared_ptr< glt::Camera > camera(new glt::Camera(20.f, 1.f, 50.f, 1.f));
	//std::shared_ptr< glt::Light  > light(new glt::Light);

	glRenderer->add("cube" , cube);/*
	glRenderer->add("light", light);
	glRenderer->add("camera", camera);*/

	/*cube->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());*/

	//if (!renderer)
	//{
	//	gameLogger->error("Error creating SDL renderer.");
	//	return;
	//}

	//if (!glContext)
	//{
	//	gameLogger->error("Error creating OpenGL context.");
	//	return;
	//}

	isRunning = true;
}

void Game::Run()
{
	while (isRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}


void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
			//Event that the system triggers when the user closes the window
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
				{
					isRunning = false;
				}
				break;
		}
	}
}

void Game::SetupScene()
{
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<Render3DSystem>();

	assetManager->AddTexture(renderer, "tank-image", "../../../assets/images/tank-panther-right.png");
	assetManager->AddTexture(renderer, "truck-image", "../../../assets/images/truck-ford-right.png");

	Entity tank = registry->CreateEntity();
	gameLogger-> info("Entity ID: " + std::to_string(tank.GetId()));

	tank.AddComponent<TransformComponent>(glm::vec3(2000, 500, 0), glm::vec3(5.0, 5.0, 5.0), 0);
	tank.AddComponent<RigidbodyComponent>(glm::vec3(50.0, 0.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32);

	Entity truck = registry->CreateEntity();
	gameLogger->info("Entity ID: " + std::to_string(truck.GetId()));

	truck.AddComponent<TransformComponent>(glm::vec3(1800, 500, 0), glm::vec3(5.0, 5.0, 5.0), 0);
	truck.AddComponent<RigidbodyComponent>(glm::vec3(50.0, 0.0, 0.0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32);
}

void Game::Update()
{
	double deltaTime = (static_cast<double>(SDL_GetTicks64()) - millisecondsPreviousFrame) * 0.001;
	millisecondsPreviousFrame = SDL_GetTicks64();

	//Ask systems to update
	registry->GetSystem<MovementSystem>().Update(deltaTime);

	//Update registry to process the entities that are waiting
	registry->Update();
}

/// <summary>
/// Renders all gameobjects
/// </summary>
void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	//Ask systems to render
	registry->GetSystem<RenderSystem>().Render(renderer, assetManager);

	// Render game objects...

	//Present the renderer / swaps render buffers.
	SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Game::ClearOpenGLContext() const
{
	if (glContext)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}
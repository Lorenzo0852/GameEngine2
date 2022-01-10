/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include "Game.h"
#include <iostream>
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include <glm/glm.hpp>
#include <spdlog/sinks/rotating_file_sink.h>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

#include <glad.h>
#include <gltk/Camera.hpp>

Game::Game()
{
	isRunning = false;
	registry = std::make_unique<Registry>();
	assetManager = std::make_unique<AssetManager>();
	gameLogger->info("Game constructor called");
}

void Game::Initialize(bool fullScreen, unsigned displayIndex)
{ 
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) //This will return != 0 if it can't initialize
	{
		gameLogger->error("Error initializing SDL");
		return;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(displayIndex, &displayMode);
	windowWidth = displayMode.w;
	windowHeight = displayMode.h;

	window = SDL_CreateWindow(
		"Unnamed game engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_BORDERLESS);

	if (!window)
	{
		gameLogger->error("Error creating SDL window.");
		return;
	}

	//-1 index means "get the default monitor/screen for the renderer".
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		gameLogger->error("Error creating SDL renderer.");
		return;
	}

	if (fullScreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

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
	registry->GetSystem<RenderSystem>().Update(renderer, assetManager);

	// Render game objects...

	//Present the renderer / swaps render buffers.
	SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
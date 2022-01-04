/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include "Game.h"
#include "../ECS/ECS.h"
#include <iostream>
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include <glm/glm.hpp>
#include <spdlog/sinks/rotating_file_sink.h>

void Game::Initialize(bool fullScreen, unsigned displayIndex)
{ 

	// Create a file rotating logger with 5mb size max and 3 rotated files
	auto max_size = 1048576 * 5;
	auto max_files = 3;
	auto file_logger = spdlog::rotating_logger_mt("Logger", "logs/logs.txt", max_size, max_files);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) //This will return != 0 if it can't initialize
	{
		file_logger->error("Error initializing SDL");
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
		file_logger->error("Error creating SDL window.");
		return;
	}

	//-1 index means "get the default monitor/screen for the renderer".
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		file_logger->error("Error creating SDL renderer.");
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

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::SetupScene()
{

}

void Game::Update()
{
	double deltaTime = (static_cast<double>(SDL_GetTicks64()) - millisecondsPreviousFrame) * 0.001;
	
	millisecondsPreviousFrame = SDL_GetTicks64();
}

/// <summary>
/// Renders all gameobjects
/// </summary>
void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

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
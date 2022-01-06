/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#pragma once

#include <sdl2/SDL.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include "../ECS/ECS.h"

class Game
{
private:
	bool isRunning;
	SDL_Window* window;
	SDL_Renderer* renderer;
	Uint64 millisecondsPreviousFrame = 0;

	// Create a file rotating logger with 5mb size max and 3 rotated files
	const int max_size = 1048576 * 5;
	const int max_files = 1;
	const std::shared_ptr<spdlog::logger> gameLogger =
		spdlog::rotating_logger_mt("GameLogger", "logs/GameLogs.txt", max_size, max_files);

	std::unique_ptr<Registry> registry;

public:
	Game();
	~Game() = default;

	void Initialize(bool fullScreen, unsigned displayIndex);
	void SetupScene();
	void Run();
	void ProcessInput();
	void Update();
	void Render();
	void Destroy();

	unsigned windowWidth;
	unsigned windowHeight;
};
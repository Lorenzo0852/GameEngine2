/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#pragma once
#include <sdl2/SDL.h>
#include <spdlog/sinks/rotating_file_sink.h>

class Game
{
private:
	bool isRunning;
	SDL_Window* window;
	SDL_Renderer* renderer;
	Uint64 millisecondsPreviousFrame = 0;
	double deltaTime;

	// Create a file rotating logger with 5mb size max and 3 rotated files
	const int max_size = 1048576 * 5;
	const int max_files = 1;
	const std::shared_ptr<spdlog::logger> file_logger =
		spdlog::rotating_logger_mt("Logger", "logs/logs.txt", max_size, max_files);

public:
	Game() = default;
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
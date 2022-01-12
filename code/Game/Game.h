/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/
#pragma once

#include <sdl2/SDL.h>
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include "../Window/Window.h"
#include <gltk/Render_Node.hpp>
#include <spdlog/sinks/rotating_file_sink.h>

class Game
{
private:
	bool isRunning;
	Window * window;
	SDL_Renderer * renderer;
	Uint64 millisecondsPreviousFrame = 0;


	SDL_GLContext glContext;
	std::unique_ptr< glt::Render_Node > glRenderer;

	// Create a file rotating logger with 5mb size max and 3 rotated files
	const int max_size = 1048576 * 5;
	const int max_files = 1;
	std::shared_ptr<spdlog::logger> gameLogger;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetManager> assetManager;

public:
	Game();
	~Game() = default;

	void Initialize(Window& window);
	void ClearOpenGLContext() const;
	void SetupScene();
	void Run();
	void ProcessInput();
	void Update();
	void Render();
	void Destroy();

	unsigned windowWidth;
	unsigned windowHeight;
};
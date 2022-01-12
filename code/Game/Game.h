/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/
#pragma once

#include <sdl2/SDL.h>
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include "../Window/Window.h"
#include <gltk/Render_Node.hpp>

class Game
{
private:
	bool isRunning;
	Window * window;
	SDL_Renderer * renderer;
	Uint64 millisecondsPreviousFrame = 0;

	SDL_GLContext glContext;
	std::unique_ptr< glt::Render_Node > glRenderer;

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
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

//Idea nombre engine:
// ARGE? -> Awesome Raw Game Engine
// NIA Engine -> No Idea at All Engine
// HSLM Engine -> Hecho Sobre La Marcha Engine
//SPDLOG_LEVEL_TRACE to enable traces
//SPDLOG_LEVEL_OFF to disable traces
#include <iostream>
#include "Game/Game.h"
#include "Window/Window.h"
#include "Kernel/Kernel.h"
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include "Logger/Logger.h"

// SDL Requires a number of arguments and an array of the actual parameters in the main function.
int main(int args, char* argv[])
{	
	// Create a file rotating logger with 5mb size max and 3 rotated files
	const int max_size = 1048576 * 5;
	const int max_files = 1;
	std::shared_ptr<spdlog::logger> gameLogger = spdlog::rotating_logger_mt("EngineLogger", "logs/EngineLogs.txt", max_size, max_files);
	spdlog::set_default_logger(gameLogger);

	//gameLogger->info("wtf");

	Window window("Unnamed game engine", 1920, 1080, false, -1);
	Kernel kernel;
	//// We create the game in the stack. We don't need the 'new' keyword for stack-only variables.
	Game game;
	////We initialize all game tasks to add them to the kernel...
	game.Initialize(window, kernel);
	game.SetupScene();
	////Then start the kernel loop.

	kernel.Execute();

	//game.Run();
	
	return 0;
}

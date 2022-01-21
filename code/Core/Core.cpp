/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include "Core.h"

void Core::StartMainGameLoop(int args, char* argv[])
{
	// Create a file rotating logger with 5mb size max and 3 rotated files
	const int max_size = 1048576 * 5;
	const int max_files = 1;
	std::shared_ptr<spdlog::logger> gameLogger = spdlog::rotating_logger_mt("EngineLogger", "logs/EngineLogs.txt", max_size, max_files);
	spdlog::set_default_logger(gameLogger);

	std::shared_ptr<EventBus> eventBus = std::make_shared<EventBus>();
	Window window("Unnamed game engine", 1920, 1080, false, -1);
	InputPollingTask inputPoller(eventBus);
	Kernel kernel;

	// We create the game in the stack. We don't need the 'new' keyword for stack-only variables.
	Game game(window, kernel, eventBus);

	//We initialize all scene specific tasks to add them to the kernel...
	game.SetupScene();
	//Then start the kernel loop.
	kernel.AddPriorizedRunningTask(inputPoller);
	kernel.AddPriorizedRunningTask(game);

	kernel.Execute();

}
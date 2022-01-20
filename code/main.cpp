/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

//Idea nombre engine:
// ARGE? -> Awesome Raw Game Engine
// NIA Engine -> No Idea at All Engine
// HSLM Engine -> Hecho Sobre La Marcha Engine
// RAW Engine -> Raw and Wacky

//Set as a preprocessor definition
//SPDLOG_LEVEL_TRACE to enable traces
//SPDLOG_LEVEL_OFF to disable traces
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <iostream>
#include "Game/Game.h"
#include "Window/Window.h"
#include "Kernel/Kernel.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include "Input/InputPollingTask.h"
#include "EventBus/EventBus.h"
#include "Core/Core.h"

// SDL Requires a number of arguments and an array of the actual parameters in the main function.
int main(int args, char* argv[])
{
	Core::StartMainGameLoop(args, argv);

	return 0;
}

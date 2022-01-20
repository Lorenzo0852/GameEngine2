#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/
#include <iostream>
#include "../Game/Game.h"
#include "../Window/Window.h"
#include "../Kernel/Kernel.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include "../Input/InputPollingTask.h"
#include "../EventBus/EventBus.h"

static class Core
{
public:
	static void StartMainGameLoop(int args, char* argv[]);
};
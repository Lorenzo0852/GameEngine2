/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

class Logger
{
	Logger(std::shared_ptr<spdlog::logger> & default_logger)
	{
		spdlog::set_default_logger(default_logger);
	}
};

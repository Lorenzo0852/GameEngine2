/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#pragma once
#include <spdlog/sinks/rotating_file_sink.h>

class Logger
{
private:
	// Create a file rotating logger with 5mb size max and 1 rotated file
	const int max_size = 1048576 * 5;
	const int max_files = 1;
public:
	const std::shared_ptr<spdlog::logger> file_logger =
		spdlog::rotating_logger_mt("Logger", "logs/logs.txt", max_size, max_files);

	Logger()  =	default;
	~Logger() = default;
};
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/sinks/basic_file_sink.h>

namespace KuchCraft {

	std::shared_ptr<spdlog::logger> Log::s_Logger;
	spdlog::sink_ptr Log::s_FileSink;

	void Log::Init()
	{
#ifdef KC_DIST
		s_FileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("KuchCraft.log", false);
#else
		s_FileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("KuchCraft.log", true);
#endif
		s_FileSink->set_pattern("[%T] [%l] : %v");

		s_Logger = std::make_shared<spdlog::logger>("KuchCraft", s_FileSink);
		s_Logger->set_level(spdlog::level::info);

		spdlog::register_logger(s_Logger);
	}

}


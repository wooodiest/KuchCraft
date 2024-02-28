#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include <memory>

namespace KuchCraft {

	class Log
	{
	public:
		static void Init();
		
		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
		static spdlog::sink_ptr s_FileSink;

	};


#ifdef KC_DIST
	#define KC_CORE_INFO(...)
	#define KC_CORE_WARN(...)     ::KuchCraft::Log::GetLogger()->warn(__VA_ARGS__)
	#define KC_CORE_ERROR(...)    ::KuchCraft::Log::GetLogger()->error(__VA_ARGS__)
#else
	#define KC_CORE_INFO(...)     ::KuchCraft::Log::GetLogger()->info(__VA_ARGS__)
	#define KC_CORE_WARN(...)     ::KuchCraft::Log::GetLogger()->warn(__VA_ARGS__)
	#define KC_CORE_ERROR(...)    ::KuchCraft::Log::GetLogger()->error(__VA_ARGS__)
#endif
}

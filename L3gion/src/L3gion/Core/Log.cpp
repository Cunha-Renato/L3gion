#include "lgpch.h"

#include "L3gion/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace L3gion
{
	ref<spdlog::logger> Log::s_CoreLogger;
	ref<spdlog::logger> Log::s_ClientLogger;


	void Log::init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		
		s_CoreLogger = spdlog::stdout_color_mt("L3GION");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}
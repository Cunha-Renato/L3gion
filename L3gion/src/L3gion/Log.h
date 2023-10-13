#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bundled/ostream.h"

namespace L3gion
{
	class L3GION_API Log
	{
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_ClientLogger; }
	
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Formater
// FUNCTION
template <typename T>
void formatToStringIMPL()
{
	struct CustomFormatter : fmt::formatter<std::string>
	{
		template <typename FormatContext>
		auto format(const T& e, FormatContext& ctx) {
			return fmt::format_to(ctx.out(), "{}", e.toString());
		}
	};
}

// MACRO
#define FORMAT_TOSTR_IMPL(Type) \
template <> \
struct fmt::formatter<Type> : fmt::formatter<std::string> \
{ \
    template <typename FormatContext> \
    auto format(const Type& e, FormatContext& ctx) \
    { \
        return fmt::format_to(ctx.out(), "{}", e.toString()); \
    } \
};

// Core log macros
#define LG_CORE_TRACE(...)      ::L3gion::Log::getCoreLogger()->trace(__VA_ARGS__)
#define LG_CORE_INFO(...)       ::L3gion::Log::getCoreLogger()->info(__VA_ARGS__)
#define LG_CORE_WARN(...)       ::L3gion::Log::getCoreLogger()->warn(__VA_ARGS__)
#define LG_CORE_ERROR(...)      ::L3gion::Log::getCoreLogger()->error(__VA_ARGS__)
#define LG_CORE_CRITICAL(...)   ::L3gion::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define LG_TRACE(...)      ::L3gion::Log::getClientLogger()->trace(__VA_ARGS__)
#define LG_INFO(...)       ::L3gion::Log::getClientLogger()->info(__VA_ARGS__)
#define LG_WARN(...)       ::L3gion::Log::getClientLogger()->warn(__VA_ARGS__)
#define LG_ERROR(...)      ::L3gion::Log::getClientLogger()->error(__VA_ARGS__)
#define LG_CRITICAL(...)   ::L3gion::Log::getClientLogger()->critical(__VA_ARGS__)
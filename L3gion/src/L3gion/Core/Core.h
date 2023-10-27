#pragma once

#include <memory>

#ifdef LG_PLATFORM_WINDOWS

#else
	#error L3gion only supports Windows!
#endif

#ifdef LG_DEBUG
	#if defined(LG_PLATFORM_WINDOWS)
		#define LG_DEBUGBREAK() __debugbreak()
	#elif defined(LG_PLATFORM_LINUX)
		#include <signal.h>
		#define LG_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif

	#define LG_ENABLE_ASSERTS
#else
	#define HZ_DEBUGBREAK()
#endif


#ifdef LG_ENABLE_ASSERTS
	#define LG_ASSERT(x, ...) { if(!(x)) {LG_ERROR("Assertion Failed: {0}", __VA_ARGS__); LG_DEBUGBREAK(); } }
	#define LG_CORE_ASSERT(x, ...) { if(!(x)) {LG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); LG_DEBUGBREAK(); } }
#else
	#define LG_ASSERT(x, ...)
	#define LG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

namespace L3gion
{
	template<typename T>
	using ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr ref<T> createRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr scope<T> createScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}
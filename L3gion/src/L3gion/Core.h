#pragma once

#ifdef LG_PLATFORM_WINDOWS
	#ifdef LG_BUILD_DLL
		#define L3GION_API __declspec(dllexport)
	#else
		#define L3GION_API __declspec(dllimport)
	#endif
#else
	#error L3gion only supports Windows!
#endif

#ifdef LG_DEBUG
#define LG_ENABLE_ASSERTS
#endif // LG_DEBUG


#ifdef LG_ENABLE_ASSERTS
	#define LG_ASSERT(x, ...) { if(!(x)) {LG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define LG_CORE_ASSERT(x, ...) { if(!(x)) {LG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define LG_ASSERT(x, ...)
	#define LG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
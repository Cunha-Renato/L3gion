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
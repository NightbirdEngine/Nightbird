#pragma once

#ifdef _WIN32
	#ifdef PROJECT_BUILD
		#define NB_PROJECT_API extern "C" __declspec(dllexport)
	#else
		#define NB_PROJECT_API extern "C"
	#endif
#else
	#define NB_PROJECT_API extern "C"
#endif

NB_PROJECT_API void RegisterTypes();
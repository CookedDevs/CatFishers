#pragma once
#include <iostream>

#ifdef _ANDROID_
#include <android/log.h>
#endif // _ANDROID_


namespace CatCore
{
	inline void LogInfo(std::string info)
	{
#ifdef _ANDROID_
		__android_log_print(ANDROID_LOG_INFO, "CatFishers", "%s", info.c_str());
#else // _ANDROID_
		std::cout << info;
#endif
	}

	inline void LogError(std::string error)
	{
#ifdef _ANDROID_
		__android_log_print(ANDROID_LOG_INFO, "CatFishers", "%s", error.c_str());
#else // _ANDROID_
		std::cerr << error;
#endif
	}
}
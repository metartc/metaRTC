//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef INCLUDE_YANG_CONFIG_OS_H_
#define INCLUDE_YANG_CONFIG_OS_H_

#if _WIN32
	#define 	Yang_OS_WIN       1
#else
	#define 	Yang_OS_WIN       0
#endif

#ifdef __ANDROID__
	#define 	Yang_OS_ANDROID     1
#else
	#define 	Yang_OS_ANDROID     0
#endif

#ifdef __APPLE__
	#include <TargetConditionals.h>
	#if defined(TARGET_OS_OSX)
	#define 	Yang_OS_MAC       1
	#define 	Yang_OS_IOS       0
	#elif TARGET_OS_IPHONE
	#define 	Yang_OS_MAC       0
	#define 	Yang_OS_IOS       1
	#endif
	#define Yang_OS_APPLE         1
#else
	#define     Yang_OS_APPLE     0
	#define 	Yang_OS_MAC       0
	#define 	Yang_OS_IOS       0
#endif

#if defined(__linux__) && !Yang_OS_ANDROID
	#define 	Yang_OS_LINUX     1
#else
	#define 	Yang_OS_LINUX     0
#endif

#endif /* INCLUDE_YANG_CONFIG_OS_H_ */

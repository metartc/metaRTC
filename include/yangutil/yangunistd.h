//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANGUNISTD_H_
#define INCLUDE_YANGUTIL_YANGUNISTD_H_

#include <yang_config.h>

#ifdef _MSC_VER
#include <Windows.h>
#define yang_usleep(x) Sleep(x>1000?x/1000:1)
//void yang_usleep(int ns){
 //       Sleep(ns>1000?ns/1000:1);
//}
#define yang_sleep(x) Sleep(1000*x)
#else


#include <unistd.h>
#define yang_usleep usleep
#define yang_sleep sleep
#define yang_exit exit
#endif


#endif /* INCLUDE_YANGUTIL_YANGUNISTD_H_ */

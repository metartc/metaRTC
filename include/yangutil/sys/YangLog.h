//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangLOG_H__
#define __YangLOG_H__
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>



#include <yangutil/yangtype.h>


#define YANG_LOG_FATAL     0
#define YANG_LOG_ERROR     1
#define YANG_LOG_WARNING   2
#define YANG_LOG_INFO      3
#define YANG_LOG_DEBUG     4
#define YANG_LOG_TRACE     5
#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_error_wrap(int32_t errcode, const char *fmt, ...);
void yang_clog(int32_t level, const char *fmt, ...);
void yang_clogf(int32_t level, const char *fmt, ...);
void yang_clogf2(int32_t level, const char *fmt, ...);
void yang_setCLogFile(int32_t isSetLogFile);
void yang_setCLogFile2(int32_t isSetLogFile, char *fullpathfile);
void yang_closeCLogFile();
void yang_setCLogLevel(int32_t plevel);
#ifdef __cplusplus
}
#endif


#if Yang_OS_ANDROID
#include <android/log.h>
#define yang_fatal( fmt, ...) __android_log_print(ANDROID_LOG_FATAL,fmt, ##__VA_ARGS__)

#define yang_warn( fmt, ...) __android_log_print(ANDROID_LOG_WARN,"metaRTC",fmt, ##__VA_ARGS__)
#define yang_info( fmt, ...) __android_log_print(ANDROID_LOG_INFO,"metaRTC",fmt, ##__VA_ARGS__)
#define yang_debug( fmt, ...) __android_log_print(ANDROID_LOG_DEBUG,"metaRTC",fmt, ##__VA_ARGS__)


#define yang_debug2( fmt, ...) __android_log_print(ANDROID_LOG_DEBUG,fmt, ##__VA_ARGS__)
#define yang_info2( fmt, ...) __android_log_print(ANDROID_LOG_INFO,fmt, ##__VA_ARGS__)
#define yang_trace( fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE,"metaRTC",fmt, ##__VA_ARGS__)
#define yang_error( fmt, ...) __android_log_print(ANDROID_LOG_ERROR,"metaRTC",fmt, ##__VA_ARGS__)
#else
#define yang_fatal( fmt, ...) yang_clog(0,fmt, ##__VA_ARGS__)
#define yang_error( fmt, ...) yang_clog(1,fmt, ##__VA_ARGS__)
#define yang_warn( fmt, ...) yang_clog(2,fmt, ##__VA_ARGS__)
#define yang_info( fmt, ...) yang_clog(3,fmt, ##__VA_ARGS__)
#define yang_debug( fmt, ...) yang_clog(4,fmt, ##__VA_ARGS__)


#define yang_debug2( fmt, ...) yang_clogf(4,fmt, ##__VA_ARGS__)
#define yang_info2( fmt, ...) yang_clogf(3,fmt, ##__VA_ARGS__)
#define yang_trace( fmt, ...) yang_clogf(5,fmt, ##__VA_ARGS__)
#define yang_trace2( fmt, ...) yang_clogf2(5,fmt, ##__VA_ARGS__)
#endif
#define yang_setLogLevel(x) yang_setCLogLevel(x)
#define yang_setLogFile(x) yang_setCLogFile(x)
#define yang_setLogFile2(x,y) yang_setCLogFile2(x,y)

#define yang_closeLogFile yang_closeCLogFile
#endif


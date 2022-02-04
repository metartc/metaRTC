//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGCLOG_H_
#define INCLUDE_YANGUTIL_SYS_YANGCLOG_H_

#include <stdint.h>
#define YANG_LOG_FATAL     0
#define YANG_LOG_ERROR     1
#define YANG_LOG_WARNING   2
#define YANG_LOG_INFO      3
#define YANG_LOG_DEBUG     4
#define YANG_LOG_TRACE     5
int32_t yang_error_wrap(int32_t errcode, const char *fmt, ...);
void yang_clog(int32_t level, const char *fmt, ...);
void yang_clogf(int32_t level, const char *fmt, ...);
void yang_setCLogFile(int32_t isSetLogFile);
void yang_closeCLogFile();
void yang_setCLogLevel(int32_t plevel);
#endif /* INCLUDE_YANGUTIL_SYS_YANGCLOG_H_ */

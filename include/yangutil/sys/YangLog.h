#ifndef __YangLOG_H__
#define __YangLOG_H__
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>


#include "yangutil/YangErrorCode.h"
#include <yangutil/yangtype.h>
#define YANG_LOG_FATAL     0
#define YANG_LOG_ERROR     1
#define YANG_LOG_WARNING   2
#define YANG_LOG_INFO      3
#define YANG_LOG_DEBUG     4
#define YANG_LOG_TRACE     5

class YangCLog {
public:
	YangCLog();
	virtual ~YangCLog();
	//int32_t neednl;

public:
	static void log(int32_t level,  const char *format, ...);
	static void logf(int32_t level,  const char *format, ...);
	static void setLogLevel(int32_t plevel);
	static void setLogFile(int32_t isSetLogFile);
	static void closeLogFile();

	static int32_t m_hasLogFile;
private:
	static int32_t logLevel;


};

int32_t yang_error_wrap(int32_t errcode, const char *fmt, ...);
#define yang_fatal( fmt, ...) YangCLog::log(0,fmt, ##__VA_ARGS__)
#define yang_error( fmt, ...) YangCLog::log(1,fmt, ##__VA_ARGS__)
#define yang_warn( fmt, ...) YangCLog::log(2,fmt, ##__VA_ARGS__)
#define yang_info( fmt, ...) YangCLog::log(3,fmt, ##__VA_ARGS__)
#define yang_debug( fmt, ...) YangCLog::log(4,fmt, ##__VA_ARGS__)


#define yang_debug2( fmt, ...) YangCLog::logf(4,fmt, ##__VA_ARGS__)
#define yang_info2( fmt, ...) YangCLog::logf(3,fmt, ##__VA_ARGS__)
#define yang_trace( fmt, ...) YangCLog::logf(5,fmt, ##__VA_ARGS__)

#define yang_setLogLevle(x) YangCLog::setLogLevel(x)
#define yang_setLogFile(x) YangCLog::setLogFile(x)

#define yang_closeLogFile YangCLog::closeLogFile
#endif

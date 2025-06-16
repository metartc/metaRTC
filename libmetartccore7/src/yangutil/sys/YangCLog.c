//
// Copyright (c) 2019-2025 yanggaofeng
//

#include <yangutil/sys/YangLog.h>

#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

#if Yang_OS_ANDROID || Yang_OS_IOS
#define Yang_Enable_Logfile 0
#else
#define Yang_Enable_Logfile 1
#endif
#if Yang_OS_WIN
#include <io.h>
#ifdef _MSC_VER
#include <direct.h>
#endif
#else
#include <sys/time.h>
#endif

#define Yang_Log_Cachesize 1024*12
#define Yang_Log_Cachesize2 Yang_Log_Cachesize+256

static int32_t g_hasLogFile=0;
static int32_t g_logLevel=YANG_LOG_ERROR;

#if Yang_Enable_Logfile
static FILE *g_fmsg = NULL;
#endif

static char const *YANG_LOG_LEVEL_NAME[] = { "FATAL", "ERROR", "WARNING",
		"INFO", "DEBUG", "TRACE" };

void yang_setCLogFile(int32_t isSetLogFile) {
#if Yang_Enable_Logfile
	char file1[300];
	char file_path_getcwd[255];

	if (g_hasLogFile)
		return;

    g_hasLogFile = isSetLogFile;

	if (isSetLogFile&&g_fmsg==NULL) {

		yang_memset(file1, 0, 300);
        yang_memset(file_path_getcwd, 0, 255);

#ifdef _MSC_VER
        if (_getcwd(file_path_getcwd, 255)) {
#else
        if (getcwd(file_path_getcwd, 255)) {
#endif
            yang_sprintf(file1, "%s/yang_log.log", file_path_getcwd);
            yang_setCLogFile2(yangtrue, file1);
		}

	}
#endif
}

static void yang_writeFile(int32_t level,struct tm* ntm,char* buf){
	int32_t sfLen;
	char logStr[4196];
	yang_memset(logStr, 0, 4196);

	if(level==YANG_LOG_ERROR&&ntm)
		sfLen = yang_sprintf(logStr, "[%02d:%02d:%02d] Yang %s: %s\n",ntm->tm_hour,ntm->tm_min,ntm->tm_sec, YANG_LOG_LEVEL_NAME[level], buf);
	else
		sfLen = yang_sprintf(logStr, "Yang %s: %s\n", YANG_LOG_LEVEL_NAME[level], buf);

	if (g_fmsg){
		fwrite(logStr, sfLen, 1, g_fmsg);
		fflush(g_fmsg);
	}
}

static void yang_writeFile2(int32_t level,struct tm* ntm,char* buf){
	int32_t sfLen;
	char logStr[Yang_Log_Cachesize2];
	yang_memset(logStr, 0, Yang_Log_Cachesize2);

	if(level==YANG_LOG_ERROR&&ntm)
		sfLen = yang_sprintf(logStr, "[%02d:%02d:%02d] Yang %s: %s\n",ntm->tm_hour,ntm->tm_min,ntm->tm_sec, YANG_LOG_LEVEL_NAME[level], buf);
	else
		sfLen = yang_sprintf(logStr, "Yang %s: %s\n", YANG_LOG_LEVEL_NAME[level], buf);

	if (g_fmsg){
		fwrite(logStr, sfLen, 1, g_fmsg);
		fflush(g_fmsg);
	}
}

void yang_setCLogFile2(int32_t isSetLogFile, char *fullpathfile) {
#if Yang_Enable_Logfile
	g_hasLogFile = isSetLogFile;
	if (g_fmsg == NULL)
		g_fmsg = fopen(fullpathfile, "wb+");
#endif
}

void yang_closeCLogFile() {
#if Yang_Enable_Logfile
	if (g_fmsg)
		fclose(g_fmsg); // g_yangLogFile;
	g_fmsg = NULL;
#endif
}

void yang_clog(int32_t level, const char *fmt, ...) {
	time_t t_now;
	struct tm* ntm=NULL;
	char buf[4096];
	va_list args;

	if (level > g_logLevel)
		return;

	yang_memset(buf, 0, 4096);

	va_start(args, fmt);
	yang_vsnprintf(buf, 4095, fmt, args);
	va_end(args);

    if(level==YANG_LOG_ERROR){
        t_now=time(NULL);
        ntm=localtime(&t_now);
        yang_printf("[%02d:%02d:%02d] Yang %s: %s\n",ntm->tm_hour,ntm->tm_min,ntm->tm_sec,YANG_LOG_LEVEL_NAME[level], buf);

    }else{
         yang_printf("Yang %s: %s\n",YANG_LOG_LEVEL_NAME[level], buf);
    }
#if Yang_Enable_Logfile
	if (g_hasLogFile)
		yang_writeFile(level,ntm,buf);
#endif

}

void yang_clog2(int32_t level, const char *fmt, ...) {
	time_t t_now;
	struct tm* ntm=NULL;
	char buf[Yang_Log_Cachesize];
	va_list args;

	if (level > g_logLevel)
		return;

	yang_memset(buf, 0, Yang_Log_Cachesize);

	va_start(args, fmt);
	yang_vsnprintf(buf, Yang_Log_Cachesize, fmt, args);
	va_end(args);

    if(level==YANG_LOG_ERROR){
        t_now=time(NULL);
        ntm=localtime(&t_now);
        yang_printf("[%02d:%02d:%02d] Yang %s: %s\n",ntm->tm_hour,ntm->tm_min,ntm->tm_sec,YANG_LOG_LEVEL_NAME[level], buf);

    }else{
         yang_printf("Yang %s: %s\n",YANG_LOG_LEVEL_NAME[level], buf);
    }
#if Yang_Enable_Logfile
	if (g_hasLogFile)
		yang_writeFile2(level,ntm,buf);
#endif

}

int32_t yang_error_wrap(int32_t errcode, const char *fmt, ...) {
	char buf[4096];
	va_list args;
	time_t t_now=time(NULL);
	struct tm* ntm=localtime(&t_now);

	yang_memset(buf, 0, 4096);

	va_start(args, fmt);
	yang_vsnprintf(buf, 4095, fmt, args);
	va_end(args);


#if Yang_OS_ANDROID
    yang_error("Yang Error(%d): %s\n",  errcode,buf);
#else
    yang_printf("Yang Error(%d): %s\n",  errcode,buf);
#endif

#if Yang_Enable_Logfile
	if (g_hasLogFile)
		yang_writeFile(YANG_LOG_ERROR,ntm,buf);
#endif

	return errcode;
}

void yang_clogf(int32_t level, const char *fmt, ...) {

	char buf[4096];
	va_list args;

	if (level > g_logLevel)	return;

	yang_memset(buf, 0, 4096);
	va_start(args, fmt);
	yang_vsnprintf(buf, 4095, fmt, args);
	va_end(args);

	yang_printf("%s",buf);
#if Yang_Enable_Logfile
	if (g_hasLogFile)
		yang_writeFile(level,NULL,buf);
#endif
}

void yang_clogf2(int32_t level, const char *fmt, ...) {

	va_list args;
	char buf[Yang_Log_Cachesize];

	if (level > g_logLevel)
		return;

	yang_memset(buf, 0, Yang_Log_Cachesize);


	va_start(args, fmt);
	yang_vsnprintf(buf, Yang_Log_Cachesize, fmt, args);
	va_end(args);

	yang_printf("%s",buf);
#if Yang_Enable_Logfile
	if (g_hasLogFile)
		yang_writeFile2(level,NULL,buf);
#endif
}

void yang_setCLogLevel(int32_t plevel) {
	g_logLevel = plevel;
	if (g_logLevel > YANG_LOG_TRACE)
		g_logLevel = YANG_LOG_TRACE;
}


#include "yangutil/sys/YangLog.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <yangutil/yang_unistd.h>
//#include <mutex>

#ifdef _WIN32
#include <io.h>
#ifdef _MSC_VER
#include <direct.h>
#endif
#else
#include <sys/time.h>
#endif

class YangLogFile {
public:
	//std::mutex m_lock;
	FILE *fmsg = NULL;
	YangLogFile() {
		fmsg = NULL;
	}
	;
	~YangLogFile() {
		if (fmsg)
			fclose(fmsg);
		fmsg = NULL;
	}
	;
	void writeFileData(char *p, int32_t plen) {
		//if(!fmsg) return;
		//m_lock.lock();
		fwrite(p, plen, 1, fmsg);
		fflush(fmsg);
		//m_lock.unlock();
	}
	//;

};

YangLogFile *g_yangLogFile = NULL;



static char const *YANG_LOG_LEVEL_NAME[] = { "FATAL", "ERROR", "WARNING",
		"INFO", "DEBUG", "TRACE" };

YangCLog::YangCLog() {
	//neednl = 0;

}

YangCLog::~YangCLog() {

}

void YangCLog::setLogFile(int32_t isSetLogFile) {
	if (!g_yangLogFile)
		g_yangLogFile = new YangLogFile();
	if (m_hasLogFile)
		return;
	m_hasLogFile = isSetLogFile;
	if (isSetLogFile) {
		char file1[300];
		memset(file1, 0, 300);
		char file_path_getcwd[255];
        memset(file_path_getcwd, 0, 255);
#ifdef _MSC_VER
        if (_getcwd(file_path_getcwd, 255)) {
#else
        if (getcwd(file_path_getcwd, 255)) {
#endif

			sprintf(file1, "%s/yang_log.log", file_path_getcwd);
			g_yangLogFile->fmsg = fopen(file1, "wb+");

		}

	}
}
void YangCLog::closeLogFile() {
	if (g_yangLogFile)
		delete g_yangLogFile;
	g_yangLogFile = NULL;
}

void YangCLog::log(int32_t level, const char *fmt, ...) {
	if (level > logLevel)
		return;
	char buf[4096];
	memset(buf, 0, 4096);
	//int32_t len=0;
	va_list args;
	va_start(args, fmt);
	//int32_t len =
	vsnprintf(buf, 4095, fmt, args);
	va_end(args);

    if (m_hasLogFile) {
        if (!g_yangLogFile)
            g_yangLogFile = new YangLogFile();
    }
    struct tm* ntm=NULL;
    if(level==YANG_LOG_ERROR){
        time_t t_now=time(NULL);
        ntm=localtime(&t_now);
        printf("[%02d:%02d:%02d] Yang %s: %s\n",ntm->tm_hour,ntm->tm_min,ntm->tm_sec,YANG_LOG_LEVEL_NAME[level], buf);

    }else{
         printf("Yang %s: %s\n",YANG_LOG_LEVEL_NAME[level], buf);
    }

	if (m_hasLogFile) {

		char sf[4196];
		memset(sf, 0, 4196);
        int32_t sfLen=0;
        if(level==YANG_LOG_ERROR&&ntm)
             sfLen = sprintf(sf, "[%02d:%02d:%02d] Yang %s: %s\n",ntm->tm_hour,ntm->tm_min,ntm->tm_sec, YANG_LOG_LEVEL_NAME[level], buf);
        else
             sfLen = sprintf(sf, "Yang %s: %s\n", YANG_LOG_LEVEL_NAME[level], buf);
		if (g_yangLogFile->fmsg)
			g_yangLogFile->writeFileData(sf, sfLen);
	}
    ntm=NULL;

}
int32_t yang_error_wrap(int32_t errcode, const char *fmt, ...) {
	char buf[4096];
	memset(buf, 0, 4096);
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 4095, fmt, args);
	va_end(args);

    time_t t_now=time(NULL);
    struct tm* ntm=localtime(&t_now);

    printf("Yang Error(%d): %s\n",  errcode,
			buf);

	if (YangCLog::m_hasLogFile) {
		if (!g_yangLogFile)
			g_yangLogFile = new YangLogFile();
		char sf[4196];
		memset(sf, 0, 4196);
        int32_t sfLen = sprintf(sf, "[%02d:%02d:%02d] Yang Error(%d): %s\n",ntm->tm_hour,ntm->tm_min,ntm->tm_sec, errcode, buf);
		if (g_yangLogFile->fmsg)
			g_yangLogFile->writeFileData(sf, sfLen);
	}
    ntm=NULL;
	return errcode;
}

void YangCLog::logf(int32_t level, const char *fmt, ...) {
	if (level > logLevel)
		return;
	char buf[4096];
	memset(buf, 0, 4096);
	int32_t len = 0;
	va_list args;
	va_start(args, fmt);
	len = vsnprintf(buf, 4095, fmt, args);
	va_end(args);

	printf(buf);

	if (m_hasLogFile) {
		if (!g_yangLogFile)
			g_yangLogFile = new YangLogFile();
		if (g_yangLogFile->fmsg)
			g_yangLogFile->writeFileData(buf, len);
	}

}
int32_t YangCLog::logLevel = YANG_LOG_ERROR;
int32_t YangCLog::m_hasLogFile = 0;
void YangCLog::setLogLevel(int32_t plevel) {
	logLevel = plevel;
	if (logLevel > YANG_LOG_TRACE)
		logLevel = YANG_LOG_TRACE;
}


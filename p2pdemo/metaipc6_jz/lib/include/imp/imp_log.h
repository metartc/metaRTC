/*
 * IMP log func header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_LOG_H__
#define __IMP_LOG_H__

/**
 * @file
 * Log接口头文件
 */

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * define log level
 */
enum {
	IMP_LOG_LEVEL_UNKNOWN,
	IMP_LOG_LEVEL_RESERVED,
	IMP_LOG_LEVEL_VERBOSE,
	IMP_LOG_LEVEL_DEBUG,
	IMP_LOG_LEVEL_INFO,
	IMP_LOG_LEVEL_WARN,
	IMP_LOG_LEVEL_ERROR,
	IMP_LOG_LEVEL_FATAL,
	IMP_LOG_LEVEL_SILENT
};

#define IMP_LOG_LEVEL_DEFAULT IMP_LOG_LEVEL_DEBUG

/**
 * define log out
 */
#define IMP_LOG_OUT_STDOUT      0
#define IMP_LOG_OUT_LOCAL_FILE  1
#define IMP_LOG_OUT_SERVER      2
//#define IMP_LOG_OUT_DEFAULT IMP_LOG_OUT_SERVER
#define IMP_LOG_OUT_DEFAULT IMP_LOG_OUT_STDOUT

/**
 * define log option
 */
#define IMP_LOG_OP_PID_SHIFT     0
#define IMP_LOG_OP_USTIME_SHIFT  1
#define IMP_LOG_OP_MODULE_SHIFT  2
#define IMP_LOG_OP_FILE_SHIFT    3
#define IMP_LOG_OP_FUNC_SHIFT    4
#define IMP_LOG_OP_LINE_SHIFT    5


#define IMP_LOG_OP_PID      (1<<IMP_LOG_OP_PID_SHIFT)
#define IMP_LOG_OP_USTIME   (1<<IMP_LOG_OP_USTIME_SHIFT)
#define IMP_LOG_OP_MODULE   (1<<IMP_LOG_OP_MODULE_SHIFT)
#define IMP_LOG_OP_FILE     (1<<IMP_LOG_OP_FILE_SHIFT)
#define IMP_LOG_OP_FUNC     (1<<IMP_LOG_OP_FUNC_SHIFT)
#define IMP_LOG_OP_LINE     (1<<IMP_LOG_OP_LINE_SHIFT)
#define IMP_LOG_OP_NONE     0
#define IMP_LOG_OP_ALL      (IMP_LOG_OP_PID|IMP_LOG_OP_USTIME|IMP_LOG_OP_MODULE|IMP_LOG_OP_FILE|IMP_LOG_OP_FUNC|IMP_LOG_OP_LINE)

#define IMP_LOG_OP_DEFAULT  IMP_LOG_OP_ALL


/**
 * used internal
 */
void imp_log_fun(int le, int op, int out, const char* tag, const char* file, int line, const char* func, const char* fmt, ...);

/**
 * 设置打印选项
 *
 * @fn void IMP_Log_Set_Option(int op);
 *
 * @param[in] op 设置打印选项
 *
 */
void IMP_Log_Set_Option(int op);

/**
 * 获取打印选项
 *
 * @fn int IMP_Log_Get_Option(void );
 *
 * @retval 返回系统中的打印选项
 *
 */
int  IMP_Log_Get_Option(void);

/**
 * reserved interface
 */
#define IMP_LOG_PRINT(tag, le, op, fmt, ...) \
	imp_log_fun(le, op, IMP_LOG_OUT_STDOUT, tag, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define IMP_LOG_TO_FILE(tag, le, op, fmt, ...) \
	imp_log_fun(le, op, IMP_LOG_OUT_LOCAL_FILE, tag, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define IMP_LOG_TO_SERVER(tag, le, op, fmt, ...) \
	imp_log_fun(le, op, IMP_LOG_OUT_SERVER, tag, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define IMP_LOG_GET_OPTION IMP_Log_Get_Option()

/* Default log to logcat */
#define IMP_LOG IMP_LOG_TO_SERVER

/**
 * @def IMP_LOG_UNK
 * UNKNOWN等級打印接口
 */
#define IMP_LOG_UNK(tag, fmt, ...)		IMP_LOG(tag, IMP_LOG_LEVEL_UNKNOWN, IMP_LOG_GET_OPTION, fmt, ##__VA_ARGS__)
/**
 * @def IMP_LOG_VERBOSE
 * VERBOSE等級打印接口
 */
#define IMP_LOG_VERBOSE(tag, fmt, ...)		IMP_LOG(tag, IMP_LOG_LEVEL_VERBOSE, IMP_LOG_GET_OPTION, fmt, ##__VA_ARGS__)
/**
 * @def IMP_LOG_DBG
 * DEBUG等級打印接口
 */
#define IMP_LOG_DBG(tag, fmt, ...)		IMP_LOG(tag, IMP_LOG_LEVEL_DEBUG, IMP_LOG_GET_OPTION, fmt, ##__VA_ARGS__)
/**
 * @def IMP_LOG_INFO
 * INFO等級打印接口
 */
#define IMP_LOG_INFO(tag, fmt, ...)		IMP_LOG(tag, IMP_LOG_LEVEL_INFO, IMP_LOG_GET_OPTION, fmt, ##__VA_ARGS__)
/**
 * @def IMP_LOG_WAR
 * WARN等級打印接口
 */
#define IMP_LOG_WARN(tag, fmt, ...)		IMP_LOG(tag, IMP_LOG_LEVEL_WARN, IMP_LOG_GET_OPTION, fmt, ##__VA_ARGS__)
/**
 * @def IMP_LOG_ERR
 * ERROR等級打印接口
 */
#define IMP_LOG_ERR(tag, fmt, ...)		IMP_LOG(tag, IMP_LOG_LEVEL_ERROR, IMP_LOG_GET_OPTION, fmt, ##__VA_ARGS__)
/**
 * @def IMP_LOG_FATAL
 * FATAL等級打印接口
 */
#define IMP_LOG_FATAL(tag, fmt, ...)		IMP_LOG(tag, IMP_LOG_LEVEL_FATAL, IMP_LOG_GET_OPTION, fmt, ##__VA_ARGS__)
/**
 * @def IMP_LOG_SILENT
 * SILENT等級打印接口
 */
#define IMP_LOG_SILENT(tag, fmt, ...)		IMP_LOG(tag, IMP_LOG_LEVEL_SILENT, IMP_LOG_GET_OPTION, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __IMP_LOG_H__ */

/*
 * YangCFile.h
 *
 *  Created on: 2021年12月26日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_SYS_YANGCFILE_H_
#define INCLUDE_YANGUTIL_SYS_YANGCFILE_H_

#include <stdint.h>
int32_t yang_getCurpath(char* path);
int32_t yang_getLibpath(char* path);
int32_t yang_getCaFile(char* pem,char* key);



#endif /* INCLUDE_YANGUTIL_SYS_YANGCFILE_H_ */

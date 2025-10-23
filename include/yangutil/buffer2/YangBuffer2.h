/*
 * YangBuffer2.h
 *
 *  Created on: 2025年9月21日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_BUFFER2_YANGBUFFER2_H_
#define INCLUDE_YANGUTIL_BUFFER2_YANGBUFFER2_H_

#include <yangutil/yangavinfo.h>

typedef struct{
	void* session;
	int32_t (*putBuffer)(void* session,uint8_t* buffer,uint32_t nb);
	int32_t (*gutBuffer)(void* session,uint8_t* buffer,uint32_t nb);
	uint32_t (*size)(void* session);
}YangBuffer2;

#ifdef __cplusplus
extern "C"{
#endif

void yang_create_buffer2(YangBuffer2* buffer,int32_t bufferLen);
void yang_destroy_buffer2(YangBuffer2* buffer);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_YANGUTIL_BUFFER2_YANGBUFFER2_H_ */

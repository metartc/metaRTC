/*
 * YangBuffer2.c
 *
 *  Created on: 2025年9月21日
 *      Author: yang
 */

#include <yangutil/buffer2/YangBuffer2.h>
#include <yangutil/sys/YangThread.h>
typedef struct{
	yang_thread_mutex_t lock;
	uint32_t maxSize;
	uint32_t size;
	uint32_t pos;
	uint8_t* buffer;
}YangBufferSession;


static uint32_t yang_size(void* psession){
	YangBufferSession* session=(YangBufferSession*)psession;
	if(session==NULL)
		return 0;

	return session->size;
}

static int32_t yang_putBuffer(void* psession,uint8_t* buffer,uint32_t nb){
	YangBufferSession* session=(YangBufferSession*)psession;
	if(session==NULL || buffer==NULL || nb==0)
		return 1;

	if(nb+session->size>session->maxSize)
		return 1;

	yang_thread_mutex_lock(&session->lock);
	if(session->pos+session->size+nb>=session->maxSize){
		memmove(session->buffer,session->buffer+session->pos,session->size);
		session->pos=0;
	}
	memcpy(session->buffer+session->pos+session->size,buffer,nb);

	session->size+=nb;
	yang_thread_mutex_unlock(&session->lock);
	return Yang_Ok;
}
static int32_t yang_getBuffer(void* psession,uint8_t* buffer,uint32_t nb){
	YangBufferSession* session=(YangBufferSession*)psession;

	if(session==NULL || buffer==NULL || nb==0)
		return 1;

	if(nb>session->size)
		return 1;

	yang_thread_mutex_lock(&session->lock);
	memcpy(buffer,session->buffer+session->pos,nb);

	session->size-=nb;
	if(session->size==0)
		session->pos=0;
	else
		session->pos+=nb;
	yang_thread_mutex_unlock(&session->lock);
	return Yang_Ok;
}

void yang_create_buffer2(YangBuffer2* buffer,int32_t bufferLen) {
	YangBufferSession* session;
	if(buffer==NULL)
		return;

	session=(YangBufferSession*)yang_calloc(sizeof(YangBufferSession),1);
	buffer->session=session;
	session->maxSize=bufferLen;
	session->size=0;
	session->pos=0;
	session->buffer=(uint8_t*)malloc(bufferLen);
    yang_thread_mutex_init(&session->lock,NULL);

	buffer->gutBuffer=yang_getBuffer;
	buffer->putBuffer=yang_putBuffer;
	buffer->size=yang_size;

}

void yang_destroy_buffer2(YangBuffer2* buffer) {
	YangBufferSession* session;
	if(buffer==NULL)
		return;

	session=(YangBufferSession*)buffer->session;
	yang_free(session->buffer);
	yang_thread_mutex_destroy(&session->lock);
	yang_free(buffer->session);
}

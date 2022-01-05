/*
 * InvokeList.cpp
 *
 *  Created on: 2019年9月27日
 *      Author: yang
 */

#include <yangrtmp/YangInvokeBuffer.h>

#include "stddef.h"
#include "memory.h"
#include "malloc.h"
#define Yang_Invoke_len 512

YangInvokeBuffer::YangInvokeBuffer() {
	index=0;
	tmp=NULL;
	tmp1=NULL;
	a_len=Yang_Invoke_len;
	pac_len=sizeof(RTMPPacket);
	newIndex=0;
	size=0;
	m_cache_num=50;
	//m_cache_num_cap=m_cache_num*a_len;
	//memset(&vkpac,0,sizeof(VokePacket));
	//memset(&vkpac2,0,sizeof(VokePacket));
	cache=(char*)malloc((a_len)*m_cache_num);
}

YangInvokeBuffer::~YangInvokeBuffer() {
	// TODO Auto-generated destructor stub
}

void YangInvokeBuffer::reset(){
	index=0;
	newIndex=0;
	size=0;

}
void YangInvokeBuffer::resetIndex(){
	index=0;
	newIndex=0;
	size=0;
}
void YangInvokeBuffer::putPacket(RTMPPacket *pac)
{
    tmp=cache+(index*a_len);
    index++;
    memcpy(tmp,pac,pac_len);
  // int32_t tlen= pac->m_nBodySize+RTMP_MAX_HEADER_SIZE;
  // if(tlen+pac_len>Yang_Invoke_len){
	//   printf("\nexcced.Error.................Invoke size....................%d\n",tlen);
	//   tlen=Yang_Invoke_len-pac_len;
  // }
   if(pac->m_body) memcpy(tmp+pac_len,pac->m_body,pac->m_nBodySize);
    if(index==m_cache_num) index=0;
	size++;
}

void YangInvokeBuffer::getPacket(RTMPPacket *pac)
{
            tmp1=cache+(newIndex*a_len);
            memcpy(pac,tmp1,pac_len);
            if(pac->m_nBodySize>0)    pac->m_body=tmp1+pac_len;
           // memcpy(p-18,tmp1+pac_len,vkpac2.m_nBodySize+18);
            newIndex++;
            if(newIndex==m_cache_num) newIndex=0;
            size=size-1;
 }

/*
 * YangAudioDecoder.cpp
 *
 *  Created on: 2020年9月3日
 *      Author: yang
 */

#include "yangdecoder/YangAudioDecoder.h"
#include <stdio.h>
YangAudioDecoder::YangAudioDecoder() {
	m_context = NULL;
	m_isInit = 0;
	m_frameSize = 0;
	m_uid=-1;
	m_alen=0;
	m_dstBuffer=new uint8_t[4096];
	m_dstLen=0;
}

YangAudioDecoder::~YangAudioDecoder() {
	m_context = NULL;
	if(m_dstBuffer) delete[] m_dstBuffer;
	m_dstBuffer=NULL;
}


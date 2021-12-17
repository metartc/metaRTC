/*
 * YangAecBase.cpp
 *
 *  Created on: 2019年11月16日
 *      Author: yang
 */

#include <yangavutil/audio/YangAecBase.h>

YangAecBase::YangAecBase() {
	m_channel=2;
	m_frameSize=1024;
	m_sampleRate=44100;
	m_echoPath=20;
}

YangAecBase::~YangAecBase() {

}



/*
 * YangAudioPreProcess.cpp
 *
 *  Created on: 2020年9月29日
 *      Author: yang
 */

#include <yangavutil/audio/YangPreProcess.h>


YangPreProcess::YangPreProcess() {
	m_channel=2;
	m_frameSize=1024;
	m_sampleRate=44100;
}

YangPreProcess::~YangPreProcess() {
	// TODO Auto-generated destructor stub
}


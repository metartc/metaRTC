/*
 * YangAudioFactory.cpp
 *
 *  Created on: 2020年9月29日
 *      Author: yang
 */

#include <yangavutil/audio/YangAudioFactory.h>

#include "YangAecSpeex.h"
#include "YangPreProcessSpeex.h"

YangAudioFactory::YangAudioFactory() {
	// TODO Auto-generated constructor stub

}

YangAudioFactory::~YangAudioFactory() {
	// TODO Auto-generated destructor stub
}

YangAecBase* YangAudioFactory::createAec(){
	return new YangAecSpeex();
}
YangPreProcess* YangAudioFactory::createPreProcess(){
	return new YangPreProcessSpeex();
}

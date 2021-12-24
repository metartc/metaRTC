/*
 * YangAudioFactory.h
 *
 *  Created on: 2020年9月29日
 *      Author: yang
 */

#ifndef YANGAUDIOPROC_YANGAUDIOFACTORY_H_
#define YANGAUDIOPROC_YANGAUDIOFACTORY_H_
#include <yangavutil/audio/YangAecBase.h>
#include <yangavutil/audio/YangPreProcess.h>

class YangAudioFactory {
public:
	YangAudioFactory();
	virtual ~YangAudioFactory();
	YangAecBase* createAec();
	YangPreProcess* createPreProcess();
};

#endif /* YANGAUDIOPROC_YANGAUDIOFACTORY_H_ */

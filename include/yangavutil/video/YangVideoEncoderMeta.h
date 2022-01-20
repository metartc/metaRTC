/*
 * YangVideoEncoderMeta.h
 *
 *  Created on: 2020年11月14日
 *      Author: yang
 */

#ifndef YANGENCODER_INCLUDE_YANGVIDEOENCODERMETA_H_
#define YANGENCODER_INCLUDE_YANGVIDEOENCODERMETA_H_
#include <stdint.h>
#include <yangutil/yangavinfotype.h>

class YangVideoEncoderMeta {
public:
	YangVideoEncoderMeta();
	virtual ~YangVideoEncoderMeta();
	virtual void yang_initVmd(YangVideoMeta *p_vmd, YangVideoInfo *p_config, YangVideoEncInfo *penc)=0;
};

#endif /* YANGENCODER_INCLUDE_YANGVIDEOENCODERMETA_H_ */

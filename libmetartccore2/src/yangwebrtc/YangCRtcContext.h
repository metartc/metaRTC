/*
 * YangCRtcContext.h
 *
 *  Created on: 2021年12月28日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_
#define SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_
#include <yangwebrtc/YangRtcContextH.h>
void yang_init_rtcContext(YangRtcContext* context);
void yang_destroy_rtcContext(YangRtcContext* context);

bool yang_has_ssrc(YangRtcTrack* track,uint32_t ssrc);



#endif /* SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_ */

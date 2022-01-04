/*
 * YangCRtcContext.h
 *
 *  Created on: 2021年12月28日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_
#define SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_
#include <yangwebrtc/YangCRtcContext_H.h>
void yang_init_rtcContext(struct YangRtcContext* context);
void yang_destroy_rtcContext(struct YangRtcContext* context);

bool yang_has_ssrc(struct YangRtcTrack* track,uint32_t ssrc);



#endif /* SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_ */

/*
 * YangCRtcAudioRecvTrack.h
 *
 *  Created on: 2022年1月3日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCRTCAUDIORECVTRACK_H_
#define SRC_YANGWEBRTC_YANGCRTCAUDIORECVTRACK_H_

#include <yangwebrtc/YangRecvTrack.h>


void yang_init_recvaudioTrack(YangRtcContext* context,YangRecvTrack* audiorecv,YangRtpBuffer *rtpBuffer);
void yang_destroy_recvaudioTrack(YangRecvTrack* audiorecv);
int32_t yang_recvaudioTrack_on_rtp(YangRtcContext* context,YangRtpPacket *ppkt);
int32_t yang_recvaudioTrack_check_send_nacks(YangRtcContext* context,YangRecvTrack* audiorecv);



#endif /* SRC_YANGWEBRTC_YANGCRTCAUDIORECVTRACK_H_ */

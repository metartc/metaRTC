/*
 * YangCVideoRecvTrack.h
 *
 *  Created on: 2022年1月3日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCVIDEORECVTRACK_H2_
#define SRC_YANGWEBRTC_YANGCVIDEORECVTRACK_H2_

#include <yangwebrtc/YangRecvTrack.h>



void yang_init_recvvideoTrack(YangRtcContext* context,YangVideoRecvTrack* videorecv,YangRtpBuffer *rtpBuffer);
void yang_destroy_recvvideoTrack(YangVideoRecvTrack* videorecv);
int32_t yang_recvvideoTrack_on_rtp(YangRtcContext* context,YangVideoRecvTrack* videorecv,YangRtpPacket *pkt);
int32_t yang_recvvideoTrack_check_send_nacks(YangRtcContext* context,YangVideoRecvTrack* videorecv);





#endif /* SRC_YANGWEBRTC_YANGCVIDEORECVTRACK_H_ */

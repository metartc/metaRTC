/*
 * YangCH264RecvTrack.h
 *
 *  Created on: 2022年1月3日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCH264RECVTRACK_H_
#define SRC_YANGWEBRTC_YANGCH264RECVTRACK_H_

#include <yangwebrtc/YangRecvTrack.h>


void yang_init_h264recv(YangRtcContext* context,YangH264RecvTrack* recv,YangRtpBuffer *rtpBuffer);
void yang_destroy_h264recv(YangH264RecvTrack* recv);
int32_t yang_h264recv_on_rtp(YangRtcContext* context,YangH264RecvTrack* recv,YangRtpPacket *src);



#endif /* SRC_YANGWEBRTC_YANGCH264RECVTRACK_H_ */

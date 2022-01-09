/*
 * YangCH265RecvTrack.h
 *
 *  Created on: 2022年1月3日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCH265RECVTRACK_H_
#define SRC_YANGWEBRTC_YANGCH265RECVTRACK_H_

#include <yangwebrtc/YangRecvTrack.h>






void yang_init_h265recv(YangRtcContext* context,YangH265RecvTrack* recv,YangRtpBuffer *rtpBuffer);
void yang_destroy_h265recv(YangH265RecvTrack* recv);
int32_t yang_h265recv_on_rtp(YangRtcContext* context,YangH265RecvTrack* recv,YangRtpPacket *src);



#endif /* SRC_YANGWEBRTC_YANGCH265RECVTRACK_H_ */

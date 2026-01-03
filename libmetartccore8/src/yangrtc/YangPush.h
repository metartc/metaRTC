//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef YANGRTC_YANGPUSH_H_
#define YANGRTC_YANGPUSH_H_
#include <yangrtc/YangRtcSession.h>

int32_t yang_send_avpacket(YangRtcSession *session, YangRtpPacket *pkt,	YangBuffer *pbuf);
int32_t yang_send_avpacket2(YangRtcContext *context, uint8_t* data,uint32_t length);
int32_t yang_send_nackpacket(YangRtcContext *context, char *data, int32_t nb);
void yang_push_audioPacket(YangRtcContext *context, uint32_t audioSSrc,uint8_t* data);
void yang_push_videoPacket(YangRtcContext *context, uint32_t videoSSrc,uint8_t* data,uint32_t length);

#endif /* SRC_YANGRTC_YANGPUSH_H_ */

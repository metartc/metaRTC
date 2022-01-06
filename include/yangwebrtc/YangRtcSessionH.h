/*
 * YangRtcSessionH.h
 *
 *  Created on: 2022年1月5日
 *      Author: yang
 */

#ifndef INCLUDE_YANGWEBRTC_YANGRTCSESSIONH_H_
#define INCLUDE_YANGWEBRTC_YANGRTCSESSIONH_H_
#include <yangutil/sys/YangCTimer.h>
#include <yangwebrtc/YangRtcContext.h>
#include <yangwebrtc/YangRtcEncodeCommon.h>

typedef struct {
	YangRtpBuffer *rtpBuffer;
	//YangTimer* 20ms;
	YangCTimer *tm_1s;
	YangCTimer *tm_100ms;
	YangH264RtpEncode *h264Encoder;
#if Yang_H265_Encoding
	YangH265RtpEncode *h265Encoder;
#endif

	YangRtpExtensionTypes extension_types_;

	int32_t isSendDtls;

	int32_t startRecv;
	int32_t twcc_id_;

	int32_t isSendStun;
	bool activeState;

	YangRtcContext *context;
	YangRtcPublishStream *pubStream;
	YangRtcPlayStream *playStream;
	YangRtpPacket pushPacket;
	YangVideoCodec codec;

} YangRtcSession;

#endif /* INCLUDE_YANGWEBRTC_YANGRTCSESSIONH_H_ */

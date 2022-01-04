





#ifndef YANGWEBRTC_YANGRTCSESSION_H_
#define YANGWEBRTC_YANGRTCSESSION_H_
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtcpCommon.h>
#include <yangrtp/YangRtcpPsfbCommon.h>

#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangSsl.h>

#include <yangutil/sys/YangCTimer.h>
#include <yangwebrtc/YangRtcContext.h>
#include <yangwebrtc/YangH264RtpEncode.h>
#include <yangwebrtc/YangH265RtpEncode.h>



typedef struct YangRtcSession{
	YangRtpBuffer* rtpBuffer;
	    //YangTimer* 20ms;
		YangCTimer* tm_1s;
		YangCTimer* tm_100ms;
		YangH264RtpEncode *h264Encoder;
	#if Yang_H265_Encoding
		YangH265RtpEncode *h265Encoder;
	#endif
		//char *stunBuffer;
		//int32_t stunBufferLen;

		YangRtpExtensionTypes extension_types_;

		int32_t isSendDtls;

		int32_t startRecv;
		int32_t twcc_id_;

		int32_t isSendStun;
	    bool activeState;

	    YangRtcContext* context;
		YangRtpPacket pushPacket;
		YangVideoCodec codec;

}YangRtcSession;
#ifdef __cplusplus
extern "C"{
#include <yangwebrtc/YangCRtcSession.h>
}
#else
#include <yangwebrtc/YangCRtcSession.h>
#endif

#endif /* YANGWEBRTC_YANGRTCSESSION_H_ */

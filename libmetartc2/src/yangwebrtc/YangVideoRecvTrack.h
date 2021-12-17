#ifndef SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK1_H_
#define SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK1_H_
#include <yangwebrtc/YangRecvTrack.h>
#include <yangwebrtc/YangH264RecvTrack.h>
#include <yangwebrtc/YangH265RecvTrack.h>
#include <yangrtp/YangRtpSTAPPayload.h>




class YangVideoRecvTrack: public YangRecvTrack ,public YangRtcMessageNotify{
public:
	YangVideoRecvTrack(int32_t uid, YangRtcContext *conf,
			YangRtcSessionI *session, YangRtcTrack *stream_descs,
			YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque);
	virtual ~YangVideoRecvTrack();

public:
	 int32_t on_rtp(YangRtpPacket *pkt);
	 int32_t check_send_nacks();
	 int32_t notify(int puid,YangRtcMessageType mess);
	 void setReceiveCallback(YangReceiveCallback *cbk);
private:
	 YangH264RecvTrack* m_h264Track;
	 YangH265RecvTrack* m_h265Track;
};
#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */

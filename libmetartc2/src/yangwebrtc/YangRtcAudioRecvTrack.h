#ifndef SRC_YANGWEBRTC_YANGRTCAUDIORECVTRACK_H_
#define SRC_YANGWEBRTC_YANGRTCAUDIORECVTRACK_H_
#include <yangwebrtc/YangRecvTrack.h>
#include <yangwebrtc/YangRtcSessionI.h>
class YangRtcAudioRecvTrack: public YangRecvTrack
{
public:
	YangRtcAudioRecvTrack(int32_t uid, YangRtcContext *conf,
			YangRtcSessionI *session, YangRtcTrack *track_desc,
			YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque);
	virtual ~YangRtcAudioRecvTrack();


public:
	int32_t on_rtp(YangRtpPacket *pkt);
	int32_t on_mixrtp(YangRtpPacket *pkt);
	YangRtpPacket *get_audiortp(YangRtpPacket *pkt);
	int32_t check_send_nacks();
	void setReceiveCallback(YangReceiveCallback *cbk);
	//size_t m_audioCacheSize;
private:
	//map<int64_t,YangFrame*> m_audioMap;
	YangRtpBuffer *m_aduioBuffer;
	YangReceiveCallback *m_recvcb;
};

#endif /* SRC_YANGWEBRTC_YANGRTCAUDIORECVTRACK_H_ */

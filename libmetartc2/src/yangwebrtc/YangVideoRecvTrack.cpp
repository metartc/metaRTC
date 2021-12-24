#include <yangwebrtc/YangVideoRecvTrack.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangavutil/video/YangMeta.h>
YangVideoRecvTrack::YangVideoRecvTrack(int32_t uid, YangRtcContext *conf,
		YangRtcSessionI *session, YangRtcTrack *track_desc,
		YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque) :
		YangRecvTrack(uid, conf, session, track_desc, rtpBuffer, pmixque, false) {

	if(conf&&conf->context&&conf->streamConf)
		conf->context->streams.setRtcMessageNotify(conf->streamConf->uid,this);
	 m_h264Track=NULL;
	 m_h265Track=NULL;

	 if(conf->codec_=="h264") m_h264Track=new YangH264RecvTrack(uid, conf, session, track_desc, rtpBuffer, pmixque);
	 if(conf->codec_=="h265") m_h265Track=new YangH265RecvTrack(uid, conf, session, track_desc, rtpBuffer, pmixque);
}


YangVideoRecvTrack::~YangVideoRecvTrack() {
	yang_delete(m_h264Track);
	yang_delete(m_h265Track);
}



void YangVideoRecvTrack::setReceiveCallback(YangReceiveCallback *cbk) {
	if(m_h264Track) m_h264Track->setReceiveCallback(cbk);
	if(m_h265Track) m_h265Track->setReceiveCallback(cbk);
}
int32_t YangVideoRecvTrack::on_rtp(YangRtpPacket *src) {
	int32_t err = Yang_Ok;
	if(m_h264Track) m_h264Track->on_rtp(src);
	if(m_h265Track) m_h265Track->on_rtp(src);
	return err;
}

int32_t YangVideoRecvTrack::check_send_nacks() {
	int32_t err = Yang_Ok;

	uint32_t timeout_nacks = 0;

	if ((err = do_check_send_nacks(timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "video");
	}

	if (timeout_nacks == 0) {
		return err;
	}

	return err;
}


int32_t YangVideoRecvTrack::notify(int puid,YangRtcMessageType mess){

	if(mess==YangRTC_Decoder_Input) {
		if(m_h264Track) m_h264Track->m_hasRequestKeyframe=false;
		if(m_h265Track) m_h265Track->m_hasRequestKeyframe=false;
		return m_session->send_rtcp_fb_pli(m_track_desc->ssrc_);
	}

	return Yang_Ok;
}


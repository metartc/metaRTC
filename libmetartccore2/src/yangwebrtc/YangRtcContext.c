#include <yangwebrtc/YangRtcContext.h>
#include <yangstream/YangStreamType.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>

void yang_init_rtcContext(struct YangRtcContext *context) {
	if (context == NULL)
		return;
	context->codec = Yang_VED_264;

	context->audioSsrc = yang_generate_ssrc();
	context->videoSsrc = yang_generate_ssrc();
	context->context = (struct YangAVContex*) calloc(
			sizeof(struct YangAVContex), 1);
	yang_trace("\naudiossrc===%u,videossrc==%u..........", context->audioSsrc,
			context->videoSsrc);
	context->state = 0;
	memset(&context->srtp, 0, sizeof(context->srtp));
	context->cer = (struct YangCertificate*) malloc(
			sizeof(struct YangCertificate));
	memset(context->cer, 0, sizeof(struct YangCertificate));
	yang_init_certificate(context->cer);

	context->udp = (struct YangUdpHandle*) malloc(sizeof(struct YangUdpHandle));
	memset(context->udp, 0, sizeof(YangUdpHandle));

	context->dtls = (struct YangRtcDtls*) malloc(sizeof(struct YangRtcDtls));
	memset(context->dtls, 0, sizeof(YangRtcDtls));

	context->dtls->udp = context->udp;
	context->dtls->srtp = &context->srtp;
	context->dtls->cer = context->cer;

}
void yang_destroy_rtcContext(YangRtcContext *context) {
	if (context == NULL)
		return;
	context->streamConf = NULL;
	//context->context = NULL;
	yang_free(context->context);
	yang_free(context->remote_audio);
	yang_free(context->remote_video);
	//yang_delete(context->source);
	yang_destroy_certificate(context->cer);

	yang_free(context->stun.data);
	if (context->udp)
		yang_stop_rtcudp(context->udp);
	yang_destroy_rtcudp(context->udp);
	yang_free(context->udp);

	yang_destroy_rtcdtls(context->dtls);
	yang_free(context->dtls);

	yang_destroy_rtcpublish(context->pubStream);
	yang_free(context->pubStream);

	yang_destroy_rtcplay(context->playStream);
	yang_free(context->playStream);
}
bool yang_has_ssrc(YangRtcTrack *track, uint32_t ssrc) {
	if (ssrc == track->ssrc_ || ssrc == track->rtx_ssrc_
			|| ssrc == track->fec_ssrc_) {
		return true;
	}

	return false;
}
/**
 YangRtcTrack::YangRtcTrack() {
 ssrc_ = 0;
 fec_ssrc_ = 0;
 rtx_ssrc_ = 0;
 }
 YangRtcTrack::~YangRtcTrack() {

 }

 //YangRtcTrack* YangRtcTrack::copy() {
 //	YangRtcTrack *cp = new YangRtcTrack();
 //
 //	cp->type_ = type_;
 //	cp->id_ = id_;
 //	cp->ssrc_ = ssrc_;
 //	cp->fec_ssrc_ = fec_ssrc_;
 //	cp->rtx_ssrc_ = rtx_ssrc_;
 //	cp->extmaps_ = extmaps_;
 //	return cp;
 //}
 //int32_t YangRtcTrack::get_rtp_extension_id(std::string uri) {
 //	for (std::map<int, std::string>::iterator it = extmaps_.begin();
 //			it != extmaps_.end(); ++it) {
 //		if (uri == it->second) {
 //			return it->first;
 //		}
 //	}
 //
 //	return 0;
 //}
 bool YangRtcTrack::has_ssrc(uint32_t ssrc) {

 if (ssrc == ssrc_ || ssrc == rtx_ssrc_ || ssrc == fec_ssrc_) {
 return true;
 }

 return false;
 }

 //void YangRtcTrack::add_rtp_extension_desc(int32_t id, std::string uri) {
 //	extmaps_[id] = uri;
 //}
 //
 //void YangRtcTrack::del_rtp_extension_desc(std::string uri) {
 //	for (std::map<int, std::string>::iterator it = extmaps_.begin();
 //			it != extmaps_.end(); ++it) {
 //		if (uri == it->second) {
 //			extmaps_.erase(it++);
 //			break;
 //		}
 //	}
 //}
 void YangRtcTrack::set_rtx_ssrc(uint32_t ssrc) {
 rtx_ssrc_ = ssrc;
 }

 void YangRtcTrack::set_fec_ssrc(uint32_t ssrc) {
 fec_ssrc_ = ssrc;
 }
 //
 //void YangRtcTrack::set_mid(std::string mid) {
 //	mid_ = mid;
 //}

 YangRtcSourceDescription::YangRtcSourceDescription() {
 audio_track_desc_ = NULL;
 }

 YangRtcSourceDescription::~YangRtcSourceDescription() {
 yang_delete(audio_track_desc_);

 for (int32_t i = 0; i < (int) video_track_descs_.size(); ++i) {
 yang_delete(video_track_descs_.at(i));
 }
 video_track_descs_.clear();
 }

 YangRtcSourceDescription* YangRtcSourceDescription::copy() {
 YangRtcSourceDescription *stream_desc = new YangRtcSourceDescription();

 if (audio_track_desc_) {
 stream_desc->audio_track_desc_ = audio_track_desc_->copy();
 }

 for (int32_t i = 0; i < (int) video_track_descs_.size(); ++i) {
 stream_desc->video_track_descs_.push_back(
 video_track_descs_.at(i)->copy());
 }

 return stream_desc;
 }

 YangRtcTrack* YangRtcSourceDescription::find_track_description_by_ssrc(
 uint32_t ssrc) {
 if (audio_track_desc_ && audio_track_desc_->ssrc_ == ssrc) {
 return audio_track_desc_;
 }

 for (int32_t i = 0; i < (int) video_track_descs_.size(); ++i) {
 if (video_track_descs_.at(i)->ssrc_ == ssrc) {
 return video_track_descs_.at(i);
 }
 }

 return NULL;
 }
 **/

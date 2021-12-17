#include <yangwebrtc/YangSrsRtcHandleImpl.h>
#include <yangrtp/YangAudioPayload.h>
#include <yangrtp/YangVideoPayload.h>
#include <yangwebrtc/YangRtcSdp.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangString.h>
#include <yangutil/sys/YangLog.h>

#ifdef _WIN32
#include <winsock2.h>
#define be32toh ntohl
#endif

#include <queue>
#include "YangRtcStun.h"

YangRtcHandle::YangRtcHandle(){
	m_recvcb=NULL;
	m_conf=NULL;
}

YangRtcHandle::~YangRtcHandle(){
	m_recvcb=NULL;
	m_conf=NULL;
}

void YangRtcHandle::setReceiveCallback(YangReceiveCallback* cbk){
	m_recvcb=cbk;
}
YangRtcHandle* YangRtcHandle::createRtcHandle(YangContext* pcontext){
	return new YangSrsRtcHandleImpl(pcontext);
}
YangSrsRtcHandleImpl::YangSrsRtcHandleImpl(YangContext* pcontext) {
	m_context.context=pcontext;
	m_session = NULL;
	m_udp=NULL;
	m_http=NULL;
	m_isInit=0;
}

YangSrsRtcHandleImpl::~YangSrsRtcHandleImpl() {
	if(m_context.state) disconnectServer();

	yang_delete(m_session);
	if(m_udp) m_udp->stop();
    yang_stop_thread(m_udp);
	yang_delete(m_udp);
	yang_delete(m_http);

}
int32_t YangSrsRtcHandleImpl::disconnectServer(){
	if(m_session){

		yang_trace("\n*******************webrtc disconnect *********\n");

		if(m_context.state) m_session->disconnect();
		m_context.state=0;
		yang_delete(m_session);

		yang_stop(m_udp);
	    yang_stop_thread(m_udp);

		m_isInit=0;


	}
	return Yang_Ok;
}
int32_t YangSrsRtcHandleImpl::getState(){
	return m_context.state;
}

int32_t YangSrsRtcHandleImpl::publishVideo(YangStreamCapture* p){
	if(m_session) return m_session->publishVideo(p);
	return Yang_Ok;
}
int32_t YangSrsRtcHandleImpl::publishAudio(YangStreamCapture* p){
	if(m_session) return m_session->publishAudio(p);
	return Yang_Ok;
}


void YangSrsRtcHandleImpl::init(YangStreamConfig* pconf) {

	if(m_isInit==0){
		if(m_context.streamConf==NULL) m_context.streamConf=pconf;
		m_http=new YangSrsSdp();


        m_context.streamConf->localIp=yang_getLocalInfo();

		if(m_udp==NULL) m_udp=new YangUdpHandle();
		if (m_session == NULL) 	m_session = new YangRtcSession(&m_context);

		m_sdpHandle.init(dynamic_cast<YangCertificateImpl*>(m_context.context->getCertificate()));
		m_isInit=1;
	}

}
int32_t YangSrsRtcHandleImpl::connectRtcServer(){
	init(NULL);
	static int32_t glocalport=m_context.streamConf->localPort;
	int err=Yang_Ok;
	if ((err=doHandleSignal(glocalport,m_http,m_context.streamConf->streamOptType))  == Yang_Ok) {
		m_udp->init(m_session,(char*)m_context.streamConf->serverIp.c_str(),glocalport);

		m_session->init(m_udp,m_recvcb,m_context.streamConf->streamOptType);
		m_udp->start();

	}
	glocalport++;
    return err;
}


int32_t YangSrsRtcHandleImpl::doHandleSignal(int32_t localport,YangSrsSdp* url, YangStreamOptType role) {
	int32_t err = Yang_Ok;
	char *srsSdp = new char[1024 * 12];
	char *tsdp = new char[1024 * 10];

	memset(srsSdp, 0, 1024 * 12);
	memset(tsdp, 0, 1024 * 10);

	m_sdpHandle.genLocalSdp(localport,&m_context, tsdp,role);
	m_session->setSsrc(m_context.audioSsrc,m_context.videoSsrc);

	char qt = '"';
	const char* roleStr=role==Yang_Stream_Play?"play":"publish";
	sprintf(srsSdp,
			"{%capi%c:%chttp://%s:%d/rtc/v1/%s/%c,%cstreamurl%c:%cwebrtc://%s:%d/%s/%s%c,%cclientip%c:null,%csdp%c:%c%s%c}",
			qt, qt, qt, m_context.streamConf->serverIp.c_str(), m_context.streamConf->serverPort,roleStr ,qt, qt, qt, qt, m_context.streamConf->serverIp.c_str(),
			m_context.streamConf->serverPort, m_context.streamConf->app.c_str(), m_context.streamConf->stream.c_str(), qt, qt, qt, qt, qt, qt, tsdp, qt);

	string tusdp(srsSdp);
	char apiurl[256] = { 0 };
	sprintf(apiurl, "http://%s:%d/rtc/v1/%s/", m_context.streamConf->serverIp.c_str(), m_context.streamConf->serverPort,roleStr);
	err=url->querySrs(apiurl, tusdp);

	if (err == Yang_Ok) {
		char s[1024] = { 0 };
		YangBuffer sb(s, 1024);
		//if(role==Yang_Stream_Play) yang_trace("\nSdp:%s",url->m_srs.sdp.c_str());
		createStunPacket(&url->m_srs, &sb,role);
		m_session->setStunBuffer( sb.data(), sb.pos());

	}

	yang_deleteA(srsSdp);
	yang_deleteA(tsdp);

	return err;
}

int32_t YangSrsRtcHandleImpl::createStunPacket(SrsSdpResponseType *prt, YangBuffer *stream,YangStreamOptType role) {
	YangStunPacket packet;

	m_context.remote_sdp->parse(prt->sdp);
	string tid = yang_random_str(12);
	uint32_t addr = be32toh(inet_addr(m_context.streamConf->localIp.c_str()));
	vector<string> sv = yang_split(prt->sessionid, ':');
	packet.set_message_type(BindingRequest);

	packet.set_local_ufrag(sv[1]);
	packet.set_remote_ufrag(sv[0]);
	packet.set_transcation_id(tid);
	packet.set_mapped_address(addr);
	packet.set_mapped_port(m_context.streamConf->localPort);
	if(role==Yang_Stream_Play)
		packet.encode_binding_request(m_context.remote_sdp->get_ice_pwd(),stream);
	else
		packet.encode_binding_request(m_context.remote_sdp->get_ice_pwd(),stream);

	if(role==Yang_Stream_Play) handlePlaySdp();

	return Yang_Ok;
}
extern int32_t srs_parse_h264_fmtp(const std::string& fmtp, H264SpecificParam& h264_param);
bool srs_sdp_has_h264_profile(const YangMediaPayloadType &payload_type,
		const string &profile) {
	int32_t err = Yang_Ok;

	if (payload_type.m_format_specific_param.empty()) {
		return false;
	}

	H264SpecificParam h264_param;
	if ((err = srs_parse_h264_fmtp(payload_type.m_format_specific_param,
			h264_param)) != Yang_Ok) {
		//srs_error_reset(err);
		return false;
	}

	if (h264_param.profile_level_id == profile) {
		return true;
	}

	return false;
}

// For example, 42001f 42e01f, see https://blog.csdn.net/epubcn/article/details/102802108
bool yang_sdp_has_h264_profile(const YangSdp *sdp, const string &profile) {
	for (size_t i = 0; i < sdp->m_media_descs.size(); ++i) {
		const YangMediaDesc &desc =sdp->m_media_descs[i];
		if (!desc.is_video()) {
			continue;
		}

		std::vector<YangMediaPayloadType> payloads =
				desc.find_media_with_encoding_name("H264");
		if (payloads.empty()) {
			continue;
		}

		for (std::vector<YangMediaPayloadType>::iterator it = payloads.begin();
				it != payloads.end(); ++it) {
			const YangMediaPayloadType &payload_type = *it;
			if (srs_sdp_has_h264_profile(payload_type, profile)) {
				return true;
			}
		}
	}

	return false;
}



int32_t YangSrsRtcHandleImpl::handlePlaySdp() {
	int32_t err = Yang_Ok;
	if(m_context.remote_audio==NULL) m_context.remote_audio=new YangAudioParam();
	if(m_context.remote_video==NULL) m_context.remote_video=new YangVideoParam();
	if(m_context.source==NULL) m_context.source=new YangRtcSourceDescription();
	YangRtcSourceDescription *stream_desc=m_context.source;
	if (!stream_desc) {
		return yang_error_wrap(ERROR_RTC_SDP_EXCHANGE,"stream description is NULL");
	}
	bool nack_enabled = true;
	bool twcc_enabled = true;
	// TODO: FIME: Should check packetization-mode=1 also.
	bool has_42e01f = yang_sdp_has_h264_profile(m_context.remote_sdp, "42e01f");

	for (int32_t i = 0; i < (int) m_context.remote_sdp->m_media_descs.size(); ++i) {
		const YangMediaDesc &remote_media_desc = m_context.remote_sdp->m_media_descs.at(i);

		YangRtcTrack *track_desc = new YangRtcTrack();
		YangAutoFree(YangRtcTrack, track_desc);

		//track_desc->set_direction("recvonly");
		track_desc->set_mid(remote_media_desc.m_mid);
		// Whether feature enabled in remote extmap.
		int32_t remote_twcc_id = 0;
		if (true) {
			map<int, string> extmaps = remote_media_desc.get_extmaps();
			for (map<int, string>::iterator it = extmaps.begin();
					it != extmaps.end(); ++it) {
				if (it->second == kTWCCExt) {
					remote_twcc_id = it->first;
					break;
				}
			}
		}

		if (twcc_enabled && remote_twcc_id) {
			track_desc->add_rtp_extension_desc(remote_twcc_id, kTWCCExt);
		}
		//printf("\nremote media_descs_ type========================%s",remote_media_desc.is_audio()?"audio":"video");
		if (remote_media_desc.is_audio()) {
			// TODO: check opus format specific param
			std::vector<YangMediaPayloadType> payloads =
					remote_media_desc.find_media_with_encoding_name("opus");
			if (payloads.empty()) {
				return yang_error_wrap(ERROR_RTC_SDP_EXCHANGE,
						"no valid found opus payload type");
			}

			for (int32_t j = 0; j < (int) payloads.size(); j++) {
				const YangMediaPayloadType &payload = payloads.at(j);
				//payload.
				// if the payload is opus, and the encoding_param_ is channel
				YangAudioPayload *audio_payload = new YangAudioPayload(
						payload.m_payload_type, payload.m_encoding_name,
						payload.m_clock_rate,
						::atol(payload.m_encoding_param.c_str()));
				audio_payload->set_opus_param_desc(
						payload.m_format_specific_param);
				m_context.remote_audio->encode=Yang_AED_OPUS;
				m_context.remote_audio->sample=payload.m_clock_rate;
				m_context.remote_audio->channel=atoi(payload.m_encoding_param.c_str());
				m_context.remote_audio->audioClock=payload.m_clock_rate;
				// TODO: FIXME: Only support some transport algorithms.
				for (int32_t k = 0; k < (int) payload.m_rtcp_fb.size(); ++k) {
					const string &rtcp_fb = payload.m_rtcp_fb.at(k);

					if (nack_enabled) {
						if (rtcp_fb == "nack" || rtcp_fb == "nack pli") {
							audio_payload->m_rtcp_fbs.push_back(rtcp_fb);
						}
					}
					if (twcc_enabled && remote_twcc_id) {
						if (rtcp_fb == "transport-cc") {
							audio_payload->m_rtcp_fbs.push_back(rtcp_fb);
						}
					}
				}

				track_desc->type_ = "audio";
				//printf("\naudio ssrc===%u,",track_desc->ssrc_);
				//track_desc->set_codec_payload((YangCodecPayload*) audio_payload);
				// Only choose one match opus codec.
				break;
			}
		} else if (remote_media_desc.is_video() && m_context.codec_ == "av1") {
			std::vector<YangMediaPayloadType> payloads =
					remote_media_desc.find_media_with_encoding_name("AV1X");
			if (payloads.empty()) {
				return yang_error_wrap(ERROR_RTC_SDP_EXCHANGE,
						"no found valid AV1 payload type");
			}

			for (int32_t j = 0; j < (int) payloads.size(); j++) {
				const YangMediaPayloadType &payload = payloads.at(j);

				// Generate video payload for av1.
				YangVideoPayload *video_payload = new YangVideoPayload(
						payload.m_payload_type, payload.m_encoding_name,
						payload.m_clock_rate);

				// TODO: FIXME: Only support some transport algorithms.
				for (int32_t k = 0; k < (int) payload.m_rtcp_fb.size(); ++k) {
					const string &rtcp_fb = payload.m_rtcp_fb.at(k);

					if (nack_enabled) {
						if (rtcp_fb == "nack" || rtcp_fb == "nack pli") {
							video_payload->m_rtcp_fbs.push_back(rtcp_fb);
						}
					}
					if (twcc_enabled && remote_twcc_id) {
						if (rtcp_fb == "transport-cc") {
							video_payload->m_rtcp_fbs.push_back(rtcp_fb);
						}
					}
				}

				track_desc->type_ = "video";
				//track_desc->set_codec_payload((YangCodecPayload*) video_payload);
				break;
			}
		} else if (remote_media_desc.is_video()&&remote_media_desc.find_encoding_name("H264")) {
			std::vector<YangMediaPayloadType> payloads =
					remote_media_desc.find_media_with_encoding_name("H264");
			if (payloads.empty()) {
				return yang_error_wrap(ERROR_RTC_SDP_EXCHANGE,
						"no found valid H.264 payload type");
			}

			std::deque<YangMediaPayloadType> backup_payloads;
			for (int32_t j = 0; j < (int) payloads.size(); j++) {
				const YangMediaPayloadType &payload = payloads.at(j);

				if (payload.m_format_specific_param.empty()) {
					backup_payloads.push_front(payload);
					continue;
				}
				H264SpecificParam h264_param;
				if ((err = srs_parse_h264_fmtp(payload.m_format_specific_param,
						h264_param)) != Yang_Ok) {
					// srs_error_reset(err);
					continue;
				}

				// If not exists 42e01f, we pick up any profile such as 42001f.
				bool profile_matched = (!has_42e01f
						|| h264_param.profile_level_id == "42e01f");

				// Try to pick the "best match" H.264 payload type.
				if (profile_matched && h264_param.packetization_mode == "1"
						&& h264_param.level_asymmerty_allow == "1") {
					// if the playload is opus, and the encoding_param_ is channel
					YangVideoPayload *video_payload = new YangVideoPayload(
							payload.m_payload_type, payload.m_encoding_name,
							payload.m_clock_rate);
					video_payload->set_h264_param_desc(
							payload.m_format_specific_param);
					m_context.remote_video->encode=Yang_VED_264;
					m_context.remote_video->videoClock=payload.m_clock_rate;
								//m_conf.audio->channel=atoi(payload.encoding_param_.c_str());
								// TODO: FIXME: Only support some transport algorithms.
					for (int32_t k = 0; k < (int) payload.m_rtcp_fb.size(); ++k) {
						const string &rtcp_fb = payload.m_rtcp_fb.at(k);

						if (nack_enabled) {
							if (rtcp_fb == "nack" || rtcp_fb == "nack pli") {
								video_payload->m_rtcp_fbs.push_back(rtcp_fb);
							}
						}
						if (twcc_enabled && remote_twcc_id) {
							if (rtcp_fb == "transport-cc") {
								video_payload->m_rtcp_fbs.push_back(rtcp_fb);
							}
						}
					}
					m_context.codec_ = "h264";
					track_desc->type_ = "video";

					//track_desc->set_codec_payload((YangCodecPayload*) video_payload);
					// Only choose first match H.264 payload type.
					break;
				}

				backup_payloads.push_back(payload);
			}

			// Try my best to pick at least one media payload type.
			/**if (!track_desc->media_ && !backup_payloads.empty()) {
				const YangMediaPayloadType &payload = backup_payloads.front();

				// if the playload is opus, and the encoding_param_ is channel
				YangVideoPayload *video_payload = new YangVideoPayload(
						payload.payload_type_, payload.encoding_name_,
						payload.clock_rate_);

				// TODO: FIXME: Only support some transport algorithms.
				for (int32_t k = 0; k < (int) payload.rtcp_fb_.size(); ++k) {
					const string &rtcp_fb = payload.rtcp_fb_.at(k);

					if (nack_enabled) {
						if (rtcp_fb == "nack" || rtcp_fb == "nack pli") {
							video_payload->rtcp_fbs_.push_back(rtcp_fb);
						}
					}

					if (twcc_enabled && remote_twcc_id) {
						if (rtcp_fb == "transport-cc") {
							video_payload->rtcp_fbs_.push_back(rtcp_fb);
						}
					}
				}

				//track_desc->set_codec_payload((YangCodecPayload*) video_payload);
				//yang_trace("choose backup H.264 payload type=%d",	payload.payload_type_);
			}**/

			// TODO: FIXME: Support RRTR?
			//local_media_desc.payload_types_.back().rtcp_fb_.push_back("rrtr");
		}else if (remote_media_desc.is_video()&&remote_media_desc.find_encoding_name("H265")) {
			std::vector<YangMediaPayloadType> payloads =
					remote_media_desc.find_media_with_encoding_name("H265");
			if (payloads.empty()) {
				return yang_error_wrap(ERROR_RTC_SDP_EXCHANGE,
						"no found valid H.265 payload type");
			}

			std::deque<YangMediaPayloadType> backup_payloads;
			for (int32_t j = 0; j < (int) payloads.size(); j++) {
				const YangMediaPayloadType &payload = payloads.at(j);

				if (payload.m_format_specific_param.empty()) {
					backup_payloads.push_front(payload);
					continue;
				}
				H264SpecificParam h264_param;
				if ((err = srs_parse_h264_fmtp(payload.m_format_specific_param,
						h264_param)) != Yang_Ok) {
					// srs_error_reset(err);
					continue;
				}

				// If not exists 42e01f, we pick up any profile such as 42001f.
				bool profile_matched = (!has_42e01f
						|| h264_param.profile_level_id == "42e01f");

				// Try to pick the "best match" H.264 payload type.
				if (profile_matched && h264_param.packetization_mode == "1"
						&& h264_param.level_asymmerty_allow == "1") {
					// if the playload is opus, and the encoding_param_ is channel
					YangVideoPayload *video_payload = new YangVideoPayload(
							payload.m_payload_type, payload.m_encoding_name,
							payload.m_clock_rate);
					video_payload->set_h264_param_desc(
							payload.m_format_specific_param);
					m_context.remote_video->encode=Yang_VED_265;
					m_context.remote_video->videoClock=payload.m_clock_rate;
								//m_conf.audio->channel=atoi(payload.encoding_param_.c_str());
								// TODO: FIXME: Only support some transport algorithms.
					for (int32_t k = 0; k < (int) payload.m_rtcp_fb.size(); ++k) {
						const string &rtcp_fb = payload.m_rtcp_fb.at(k);

						if (nack_enabled) {
							if (rtcp_fb == "nack" || rtcp_fb == "nack pli") {
								video_payload->m_rtcp_fbs.push_back(rtcp_fb);
							}
						}
						if (twcc_enabled && remote_twcc_id) {
							if (rtcp_fb == "transport-cc") {
								video_payload->m_rtcp_fbs.push_back(rtcp_fb);
							}
						}
					}

					m_context.codec_ = "h265";

					track_desc->type_ = "video";

					//track_desc->set_codec_payload((YangCodecPayload*) video_payload);
					// Only choose first match H.264 payload type.
					break;
				}

				backup_payloads.push_back(payload);
			}

			// Try my best to pick at least one media payload type.
			/**if (!track_desc->media_ && !backup_payloads.empty()) {
				const YangMediaPayloadType &payload = backup_payloads.front();

				// if the playload is opus, and the encoding_param_ is channel
				YangVideoPayload *video_payload = new YangVideoPayload(
						payload.payload_type_, payload.encoding_name_,
						payload.clock_rate_);

				// TODO: FIXME: Only support some transport algorithms.
				for (int32_t k = 0; k < (int) payload.rtcp_fb_.size(); ++k) {
					const string &rtcp_fb = payload.rtcp_fb_.at(k);

					if (nack_enabled) {
						if (rtcp_fb == "nack" || rtcp_fb == "nack pli") {
							video_payload->rtcp_fbs_.push_back(rtcp_fb);
						}
					}

					if (twcc_enabled && remote_twcc_id) {
						if (rtcp_fb == "transport-cc") {
							video_payload->rtcp_fbs_.push_back(rtcp_fb);
						}
					}
				}

				//track_desc->set_codec_payload((YangCodecPayload*) video_payload);
				//yang_trace("choose backup H.264 payload type=%d",	payload.payload_type_);
			}**/

			// TODO: FIXME: Support RRTR?
			//local_media_desc.payload_types_.back().rtcp_fb_.push_back("rrtr");
		}

		// TODO: FIXME: use one parse payload from sdp.

		//track_desc->create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("red"));
		//track_desc->create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("rtx"));
		//track_desc->create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("ulpfec"));

        std::string track_id;
        bool hasTrack=false;
		for (int32_t j = 0; j < (int) remote_media_desc.m_ssrc_infos.size(); ++j) {
			const YangSSRCInfo &ssrc_info = remote_media_desc.m_ssrc_infos.at(j);

			// ssrc have same track id, will be description in the same track description.
        	if (track_id != ssrc_info.m_msid_tracker) {
				YangRtcTrack *track_desc_copy = track_desc->copy();
				track_desc_copy->ssrc_ = ssrc_info.m_ssrc;
				track_desc_copy->id_ = ssrc_info.m_msid_tracker;
				track_desc_copy->msid_ = ssrc_info.m_msid;

				if (remote_media_desc.is_audio()
						&& !stream_desc->audio_track_desc_) {
					stream_desc->audio_track_desc_ = track_desc_copy;
				} else if (remote_media_desc.is_video()) {
					stream_desc->video_track_descs_.push_back(track_desc_copy);
				}
				hasTrack=true;
			}
            track_id = ssrc_info.m_msid_tracker;

        }
		//rtmp to rtc no msid
		if(!hasTrack){
			if( remote_media_desc.m_ssrc_infos.size()>0){
				const YangSSRCInfo &ssrc_info1 = remote_media_desc.m_ssrc_infos.at(0);
				YangRtcTrack *track_desc_copy = track_desc->copy();
								track_desc_copy->ssrc_ = ssrc_info1.m_ssrc;
								track_desc_copy->id_ = ssrc_info1.m_msid_tracker;
								track_desc_copy->msid_ = ssrc_info1.m_msid;
								if (remote_media_desc.is_audio()
														&& !stream_desc->audio_track_desc_) {
													stream_desc->audio_track_desc_ = track_desc_copy;
												} else if (remote_media_desc.is_video()&&stream_desc->video_track_descs_.size()==0) {
													stream_desc->video_track_descs_.push_back(track_desc_copy);
												}
			}

		}


		// set track fec_ssrc and rtx_ssrc
		for (int32_t j = 0; j < (int) remote_media_desc.m_ssrc_groups.size(); ++j) {
			const YangSSRCGroup &ssrc_group = remote_media_desc.m_ssrc_groups.at(
					j);

			YangRtcTrack *track_desc =
					stream_desc->find_track_description_by_ssrc(
							ssrc_group.m_ssrcs[0]);
			if (!track_desc) {
				continue;
			}

			if (ssrc_group.m_semantic == "FID") {
				track_desc->set_rtx_ssrc(ssrc_group.m_ssrcs[1]);
			} else if (ssrc_group.m_semantic == "FEC") {
				track_desc->set_fec_ssrc(ssrc_group.m_ssrcs[1]);
			}
		}
	}

	if(m_context.context){
		m_context.context->streams.setMediaConfig(m_context.streamConf->uid,	m_context.remote_audio,m_context.remote_video);
	}
	return err;
}


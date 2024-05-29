//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPlayTrack.h>
#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangNalu.h>

#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangPlayTrackH.h>

#include <math.h>

void yang_create_playTrack(YangRtcContext *context, YangPlayTrack *track, yangbool is_audio) {
	if (track == NULL)
		return;
	track->uid = context->streamConfig->uid;
	track->isAudio = is_audio;
	track->rtp_queue = (YangReceiveNackBuffer*) yang_calloc(1,sizeof(YangReceiveNackBuffer));
	track->nack_receiver = (YangRtpRecvNack*) yang_calloc(1,sizeof(YangRtpRecvNack));
	if (is_audio) {
		track->ssrc = context->audioTrack.ssrc;
		yang_create_nackBuffer(track->rtp_queue, 100);
		yang_create_recvnack(track->nack_receiver, track->rtp_queue, 100 * 2 / 3);
	} else {
        if(context->videoTracks.vsize>0) track->ssrc=context->videoTracks.payload[0].ssrc;
		yang_create_nackBuffer(track->rtp_queue, 1000);
		yang_create_recvnack(track->nack_receiver, track->rtp_queue, 1000 * 2 / 3);
	}
	track->last_sender_report_rtp_time = 0;
	track->last_sender_report_sys_time = 0;

	track->last_seq = 0;
	track->enableAudioBuffer=context->avinfo->rtc.enableAudioBuffer;
	track->sortBuffer=NULL;
	yang_memset(&track->audioFrame, 0, sizeof(YangFrame));
}

void yang_destroy_playTrack(YangPlayTrack *track) {
	if (track == NULL)
		return;

	yang_destroy_nackBuffer(track->rtp_queue);
	yang_free(track->rtp_queue);
	yang_destroy_recvnack(track->nack_receiver);
	yang_free(track->nack_receiver);
}

int32_t yang_playtrack_has_ssrc(YangRtcContext *context, YangPlayTrack *track,
		uint32_t ssrc) {

	if (track->isAudio)
		return yang_has_ssrc(&context->audioTrack, ssrc) ? 1 : 0;
	else{
		for(int i=0;i<context->videoTracks.vsize;i++){
			if( yang_has_ssrc(&context->videoTracks.payload[i], ssrc)) return 1;
		}
	}
	return 0;
}

void yang_playtrack_update_rtt(YangPlayTrack *track, int32_t rtt) {
	yang_recvnack_update_rtt(track->nack_receiver, rtt);

}

void yang_playtrack_update_send_report_time(YangPlayTrack *track, YangNtp *ntp,
		uint32_t rtp_time) {

	yang_memcpy(&track->last_sender_report_ntp, ntp, sizeof(YangNtp));
	track->last_sender_report_rtp_time = rtp_time;

	track->last_sender_report_sys_time = yang_update_system_time();
}

int32_t yang_playtrack_send_rtcp_rr(YangRtcContext *context,yangbool isAudio,YangPlayTrack *track) {
	int32_t err = Yang_Ok;

	uint32_t ssrc = track->ssrc;
	const uint64_t last_time = track->last_sender_report_sys_time;


	if ((err = yang_send_rtcp_rr(context, isAudio,ssrc, track->rtp_queue, last_time,
			&track->last_sender_report_ntp)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u, last_time=%lld", ssrc, last_time);
	}

	return err;
}

int32_t yang_playtrack_send_rtcp_xr_rrtr(YangRtcContext *context,
		YangPlayTrack *track) {
	int32_t err = Yang_Ok;

	if ((err = yang_send_rtcp_xr_rrtr(context, track->ssrc)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u", track->ssrc);
	}

	return err;
}


int32_t yang_playtrack_on_nack(YangRtcContext *context, YangPlayTrack *track,YangRtpPacket *pkt) {
	int32_t err = Yang_Ok;

	uint16_t seq = pkt->header.sequence;
	if (track->nack_receiver->queue.vsize > 0) {
		YangRtpNackInfo *nack_info = yang_recvnack_find(track->nack_receiver,seq);
		if (nack_info) {
			//receive lost seq
			yang_trace("\nreceive nack seq=%hu", seq);
			yang_recvnack_remove(track->nack_receiver, seq);
			return err;
		}
	}
	// insert check nack
	uint16_t nack_first = 0, nack_last = 0;

	yang_nackbuffer_update2(track->rtp_queue, seq, &nack_first, &nack_last);
	if (yang_rtp_seq_distance(nack_first, nack_last) > 0) {
		yang_trace("\nnack check: lost current seq=%u, nack range [%u, %u]", seq,nack_first, nack_last);
		yang_recvnack_insert(track->nack_receiver, nack_first, nack_last);
		yang_recvnack_check_queue_size(track->nack_receiver);

	}

//insert nack
	yang_nackbuffer_set(track->rtp_queue, seq, pkt->header.sequence);
	if (!yang_recvnack_isEmpty(track->nack_receiver)) {
		uint32_t timeout_nacks = 0;
		yang_playtrack_do_check_send_nacks(context, track, &timeout_nacks);
	}

	return err;
}

int32_t yang_playtrack_do_check_send_nacks(YangRtcContext *context,YangPlayTrack *track, uint32_t *timeout_nacks) {
	uint32_t sent_nacks = 0;
	return yang_check_send_nacks(context, track->nack_receiver, track->ssrc,&sent_nacks, timeout_nacks);

}

void yang_playtrack_receiveVideo(YangRtcContext *context,YangFrame *videoFrame) {
    if(videoFrame==NULL||videoFrame->payload==NULL||videoFrame->nb>YANG_VIDEO_ENCODE_BUFFER_LEN) return;
    uint8_t first=videoFrame->payload[0];
	uint8_t *temp = videoFrame->payload+1;
    int32_t videoLen = videoFrame->nb-1;

    videoFrame->dts=videoFrame->pts;

    videoFrame->payload = temp ;
    videoFrame->nb = videoLen;

    if(first == 0x27){
        videoFrame->frametype = YANG_Frametype_P;
        if(yang_hasH264Pframe(videoFrame->payload)) goto sendevideo;
    }

    if (first == 0x17) {

#if Yang_Rtp_I_Withmeta
    	if(context->avinfo->video.videoDecHwType==Yang_Hw_Soft){
    		videoFrame->frametype =YANG_Frametype_I;
    		goto sendevideo;
    	}
#endif

    	YangH264NaluData nalu;
    	yang_parseH264Nalu(videoFrame,&nalu);

    	if(nalu.spsppsPos>-1&&nalu.keyframePos>-1){
    		uint8_t meta[Yang_Nalu_SpsppsBufferLen] = { 0 };
    		videoFrame->payload=meta;
    		if(yang_getH264SpsppseNalu(videoFrame,temp+nalu.spsppsPos)!=Yang_Ok){
    			yang_error("get h264 nalue fail");
    			return;
    		}
    		videoFrame->frametype = YANG_Frametype_Spspps;

    		if (context &&context->streamConfig->recvCallback.receiveVideo){
    			context->streamConfig->recvCallback.receiveVideo(context->streamConfig->recvCallback.context,videoFrame);
    		}
    	}

    	if(nalu.keyframePos>-1){
    		videoFrame->payload = temp + nalu.keyframePos;
    		videoFrame->nb = videoLen - nalu.keyframePos;
    		videoFrame->frametype =YANG_Frametype_I;
    		goto sendevideo;
    	}
    }

    if( first == 0x2c){
        videoFrame->frametype = YANG_Frametype_P;
        goto sendevideo;
        return;
    }

    if ( first == 0x1c ) {

    	YangH264NaluData nalu;
    	yang_parseH265Nalu(videoFrame,&nalu);

    	if(nalu.spsppsPos>-1&&nalu.keyframePos>-1){
    		uint8_t meta[Yang_Nalu_SpsppsBufferLen] = { 0 };
    		videoFrame->payload=meta;
    		if(yang_getH265SpsppseNalu(videoFrame,temp + nalu.spsppsPos)!=Yang_Ok){
    			yang_error("get h265 nalue fail");
    			return;
    		}
    		videoFrame->frametype = YANG_Frametype_Spspps;

    		if (context && context->streamConfig->recvCallback.receiveVideo)
    			context->streamConfig->recvCallback.receiveVideo(context->streamConfig->recvCallback.context,videoFrame);
    	}
    	if(nalu.keyframePos>-1){
    		videoFrame->payload = temp + nalu.keyframePos;
    		videoFrame->nb = videoLen - nalu.keyframePos;
    		videoFrame->frametype = YANG_Frametype_I;
    		goto sendevideo;

    	}

    }

    return;
    sendevideo:
		videoFrame->uid=context->streamConfig->uid;
		if (context &&context->streamConfig->recvCallback.receiveVideo){
			context->streamConfig->recvCallback.receiveVideo(context->streamConfig->recvCallback.context,videoFrame);
		}
		return;

}

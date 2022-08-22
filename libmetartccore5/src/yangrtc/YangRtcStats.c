//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangRtcStats.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangLog.h>

void yang_stats_on_video_rtp(YangSendStats* stats,YangRtpPacket* pkt,YangBuffer* buf){
	stats->videoRtpPacketCount++;
	stats->videoRtpBytes+=yang_buffer_pos(buf);
	stats->lastVideoSendTime=yang_get_system_time();
	if(stats->videoStartTime==0)
		stats->videoStartTime=yang_get_system_time();

}

void yang_stats_on_audio_rtp(YangSendStats* stats,YangRtpPacket* pkt,YangBuffer* buf){
	stats->audioRtpPacketCount++;
	stats->audioRtpBytes+=yang_buffer_pos(buf);
	stats->lastAudioSendTime=yang_get_system_time();
	if(stats->audioStartTime==0)
		stats->audioStartTime=yang_get_system_time();

}


int32_t yang_stats_on_recvRR(yangbool isAudio,YangRecvStats* recvStats,YangSendStats* sendStats,   YangRtcpCommon *rtcp){

		YangRemoteRecvStats* stats=isAudio?&recvStats->audio:&recvStats->video;
		uint32_t rtt=0;
		uint32_t dlsr= rtcp->rb->dlsr;
		uint32_t lastSr=rtcp->rb->lsr;
		uint32_t currTime=(yang_get_system_time()-(isAudio?sendStats->audioStartTime:sendStats->videoStartTime))/1000;
		uint32_t currNtp=(yang_get_ntptime_fromms(currTime)>> 16) & 0xffffffff;

		rtt=(currNtp-lastSr-dlsr)*1000/65536;

	   // if (rtcp->rb->fraction_lost > -1.0) {
	   // 	stats->fractionLost = rtcp->rb->fraction_lost;
	   // }

	    stats->totalRtt += rtt;
	    stats->rtt = rtt>0?rtt:1;
	    stats->reportsReceived++;
	   // sendStats->videoRtpPacketCount+=rtcp->rb->lost_packets;
	    return Yang_Ok;
}


void yang_create_rtcstats(YangRtcStats* stats){
	if(stats==NULL) return;
	stats->on_videoRtp=yang_stats_on_video_rtp;
	stats->on_audioRtp=yang_stats_on_audio_rtp;
	stats->on_recvRR=yang_stats_on_recvRR;
}

void yang_destroy_rtcstats(YangRtcStats* stats){

}

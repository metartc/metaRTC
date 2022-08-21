//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangRtcStats.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangLog.h>
void yang_stats_on_video_rtp(YangSendStats* stats,YangRtpPacket* pkt,YangBuffer* buf){
	stats->videoRtpPacketCount++;
	stats->videoRtpBytes+=yang_buffer_pos(buf);
	stats->lastVideoSendTime=pkt->header.timestamp;
	if(stats->videoStartTime==0)
		stats->videoStartTime=yang_get_system_time();

}

void yang_stats_on_audio_rtp(YangSendStats* stats,YangRtpPacket* pkt,YangBuffer* buf){
	stats->audioRtpPacketCount++;
	stats->audioRtpBytes+=yang_buffer_pos(buf);
	stats->lastAudioSendTime=pkt->header.timestamp;
	if(stats->audioStartTime==0)
		stats->audioStartTime=yang_get_system_time();

}

void yang_create_rtcstats(YangRtcStats* stats){
	if(stats==NULL) return;
	stats->on_videoRtp=yang_stats_on_video_rtp;
	stats->on_audioRtp=yang_stats_on_audio_rtp;
}

void yang_destroy_rtcstats(YangRtcStats* stats){

}

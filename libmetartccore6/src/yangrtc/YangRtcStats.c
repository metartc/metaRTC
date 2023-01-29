//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangRtcStats.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangTime.h>

void yang_stats_on_pub_video_rtp(YangSendStats* stats,YangRtpPacket* pkt,YangBuffer* buf){
	stats->videoRtpPacketCount++;
	stats->videoRtpBytes+=yang_buffer_pos(buf);
	stats->lastVideoSendTime=yang_get_system_time();

	//yang_trace("\nlast send time==%" PRIu64 ,stats->lastVideoSendTime);

	if(stats->videoStartTime==0)
		stats->videoStartTime=yang_get_system_time();

}

void yang_stats_on_pub_audio_rtp(YangSendStats* stats,YangRtpPacket* pkt,YangBuffer* buf){
	stats->audioRtpPacketCount++;
	stats->audioRtpBytes+=yang_buffer_pos(buf);
	stats->lastAudioSendTime=yang_get_system_time();
	if(stats->audioStartTime==0)
		stats->audioStartTime=yang_get_system_time();

}
uint64_t yang_stats_getLostCount(YangRemoteRecvStats* stats){
	int64_t expected = (stats->cycleCount << 16) + stats->maxSn - stats->startSn + 1;
	int64_t ret=expected-stats->rtpPacketCount;
	return ret>0?(uint64_t)ret:0;
}

uint32_t yang_stats_geLostInterval(YangRemoteRecvStats* stats) {
	uint32_t lost = yang_stats_getLostCount(stats);
	if(lost<stats->lastLost) return 0;
	uint32_t ret = lost - stats->lastLost;
    stats->lastLost = lost;

    return ret;
}

int64_t yang_stats_getExpectedPacketsInterval(YangRemoteRecvStats* stats) {

    int64_t expected = (stats->cycleCount << 16) + stats->maxSn - stats->startSn + 1;
    int32_t ret = expected - stats->lastExpected;
    stats->lastExpected = expected;
    return ret;
}

uint8_t yang_stats_getFractionLost(YangRemoteRecvStats* stats){
	 int64_t expected_interval = yang_stats_getExpectedPacketsInterval(stats);
	 uint8_t fraction_lost = 0;
	 uint32_t lostInterval=0;
	    if (expected_interval) {
	    	lostInterval=yang_stats_geLostInterval(stats);
	    	fraction_lost = (uint8_t)(lostInterval << 8 / expected_interval);
	    }
	    return fraction_lost;
}

void yang_stats_on_play_rtp(YangRemoteRecvStats* stats,YangRtpPacket* pkt){

	uint16_t seq=pkt->header.sequence;

	if (stats->lastSn > 0xFF00 && seq < 0xFF && (!stats->cycleCount || stats->rtpPacketCount - stats->lastRtpPcackCount > 0x1FFF)) {
		stats->cycleCount++;
		stats->lastRtpPcackCount = stats->rtpPacketCount;
		stats->maxSn = seq;
	} else if (seq > stats->maxSn) {
		stats->maxSn = seq;
	}

	if (!stats->rtpPacketCount) {
		stats-> startSn = seq;
	} else if (!stats->cycleCount && seq < stats->startSn) {
		stats->startSn = seq;
	}

	stats->lastSn = seq;
	stats->rtpPacketCount++;

}

void yang_stats_on_play_video_rtp(YangRecvStats* stats,YangRtpPacket* pkt){
	yang_stats_on_play_rtp(&stats->video,pkt);
}
void yang_stats_on_play_audio_rtp(YangRecvStats* stats,YangRtpPacket* pkt){
	stats->audio.rtpPacketCount++;
}

int32_t yang_stats_on_recvRR(yangbool isAudio,YangRecvStats* recvStats,YangSendStats* sendStats,   YangRtcpCommon *rtcp){

		YangRemoteRecvStats* stats=isAudio?&recvStats->audio:&recvStats->video;
		uint32_t rtt=0;
		uint32_t dlsr= rtcp->rb->dlsr;
		uint32_t lastSr=rtcp->rb->lsr;
		uint32_t currTime=(yang_get_system_time()-(isAudio?sendStats->audioStartTime:sendStats->videoStartTime))/1000;
		uint32_t currNtp=(yang_get_ntptime_fromms(currTime)>> 16) & 0xffffffff;
		double fractionLost=rtcp->rb->fraction_lost/255;
		rtt=(currNtp-lastSr-dlsr)*1000/65536;

	    if (fractionLost > -1.0) {
	    	stats->fractionLost = fractionLost;
	    }

	    stats->totalRtt += rtt;
	    stats->rtt = rtt>0?rtt:1;
	    stats->reportsReceived++;
	   // sendStats->videoRtpPacketCount+=rtcp->rb->lost_packets;
	    return Yang_Ok;
}


void yang_create_rtcstats(YangRtcStats* stats){
	if(stats==NULL) return;
	stats->on_pub_videoRtp=yang_stats_on_pub_video_rtp;
	stats->on_pub_audioRtp=yang_stats_on_pub_audio_rtp;
	stats->on_play_audioRtp=yang_stats_on_play_audio_rtp;
	stats->on_play_videoRtp=yang_stats_on_play_video_rtp;

	stats->on_recvRR=yang_stats_on_recvRR;
	stats->getFractionLost=yang_stats_getFractionLost;
	stats->getLostCount=yang_stats_getLostCount;
}

void yang_destroy_rtcstats(YangRtcStats* stats){

}

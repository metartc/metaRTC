//
// Copyright (c) 2019-2025 yanggaofeng
//

#include <yangutil/yangavinfo.h>
#include <yangutil/yangframebuffer.h>

#include <yangrtc/YangPeerInfo.h>

void yang_frame_copy_nobuffer( YangFrame *src,  YangFrame *dst) {
	if (!src || !dst)		return;
	dst->mediaType = src->mediaType;
	dst->uid = src->uid;
	dst->frametype = src->frametype;
	dst->nb = src->nb;
	dst->pts = src->pts;
	dst->dts=src->dts;
}

void yang_frame_copy( YangFrame *src,  YangFrame *dst) {
	if (!src || !dst)		return;
	yang_frame_copy_nobuffer(src,dst);
	dst->payload = src->payload;
}

void yang_frame_copy_buffer( YangFrame *src,  YangFrame *dst) {
	if (!src || !dst)		return;
	yang_frame_copy_nobuffer(src,dst);
	if (src->payload && dst->payload)
		yang_memcpy(dst->payload, src->payload, src->nb);
}

void yang_avinfo_initVideo(YangVideoInfo *video) {
	video->width = 1280;
	video->height = 720;
	video->outWidth = 1280;
	video->outHeight = 720;
	video->rate = 2048;
	video->frame = 30;
	video->rotate=0;
	video->bitDepth = 8;

	video->videoCacheNum = 10;
	video->evideoCacheNum = 10;
	video->videoPlayCacheNum = 5;

	video->videoCaptureFormat = YangYuy2;
	video->videoEncoderFormat = YangI420;
	video->videoDecoderFormat = YangI420;

	video->videoEncoderType = 0;
	video->videoDecoderType = 0;
	video->videoEncHwType = 0;
	video->videoDecHwType = 0;
	video->vIndex = 0;
}

void yang_avinfo_initAudio(YangAudioInfo *audio) {
	audio->sample = 48000;
	audio->frameSize = 960;
	audio->channel = 2;
	audio->bitrate = 128;
	audio->enableMono = yangfalse;
	audio->enableAec = yangfalse;
	audio->enableAudioFec = yangtrue;
	audio->echoPath = 10;
	audio->aecBufferFrames = 0;
	audio->enableAudioHeader = yangfalse;
	audio->audioEncoderType = 3;
	audio->audioDecoderType = 3;
	audio->audioPlayType = 0;
	audio->audioCacheNum = 10;
	audio->audioCacheSize = 10;
	audio->audioPlayCacheNum = 10;

	audio->aIndex = -1;
	audio->aSubIndex = 0;
}

void yang_avinfo_initSys(YangSysInfo *sys) {
	sys->familyType=Yang_IpFamilyType_IPV4;

	sys->enableLogFile = yangfalse;
	sys->mediaServer=Yang_Server_Srs;
	sys->transType = Yang_Webrtc;
	sys->httpPort=1988;
	sys->logLevel = 1;
	yang_strcpy(sys->whipUrl, "rtc/v1/whip/?app=%s&stream=%s");
	yang_strcpy(sys->whepUrl, "rtc/v1/whip-play/?app=%s&stream=%s");
}

void yang_avinfo_initEnc(YangVideoEncInfo *enc) {
	enc->preset = 3;
	enc->level_idc = 31;
	enc->profile = 0;
	enc->keyint_max = 30;
	enc->enc_threads = 4;
	enc->gop=60;
	enc->createMeta=yangtrue;
}

void yang_avinfo_initRtc(YangRtcInfo *rtc) {
	rtc->enableSdpCandidate = yangfalse;
	rtc->sessionTimeout= 30*1000000;

	rtc->iceCandidateType = 0;

	rtc->iceServerPort=3478;

	rtc->rtcSocketProtocol=Yang_Socket_Protocol_Udp;
	rtc->turnSocketProtocol=Yang_Socket_Protocol_Udp;

	rtc->rtcPort = 1985;
	rtc->rtcLocalPort = 16000;

	yang_strcpy(rtc->iceServerIP, "127.0.0.1");
	yang_strcpy(rtc->iceUserName, "metartc");
	yang_strcpy(rtc->icePassword, "metartc");
}



void yang_init_avinfo(YangAVInfo* avinfo){

	yang_memset(&avinfo->video, 0, sizeof(YangVideoInfo));
	yang_memset(&avinfo->audio, 0, sizeof(YangAudioInfo));
	yang_memset(&avinfo->sys, 0, sizeof(YangSysInfo));
	yang_memset(&avinfo->enc, 0, sizeof(YangVideoEncInfo));
	yang_memset(&avinfo->rtc, 0, sizeof(YangRtcInfo));

	yang_avinfo_initAudio(&avinfo->audio);
	yang_avinfo_initVideo(&avinfo->video);
	yang_avinfo_initSys(&avinfo->sys);
	yang_avinfo_initEnc(&avinfo->enc);
	yang_avinfo_initRtc(&avinfo->rtc);

}

void yang_init_peerInfo(YangPeerInfo* peerInfo){
	peerInfo->familyType=Yang_IpFamilyType_IPV4;

	peerInfo->userId=2001;

	peerInfo->pushAudio.sample = 48000;
	peerInfo->pushAudio.channel = 2;
	peerInfo->pushAudio.audioEncoderType = Yang_AED_OPUS;
	peerInfo->pushAudio.enableAudioFec = yangfalse;

	peerInfo->pushVideo.width = 1280;
	peerInfo->pushVideo.height = 720;
	peerInfo->pushVideo.fps = 30;
	peerInfo->pushVideo.videoEncoderType = Yang_VED_H264;

	peerInfo->rtc.isControlled=yangfalse;
	peerInfo->rtc.enableSdpCandidate = yangfalse;
	peerInfo->rtc.sessionTimeout= 30*1000000;

	peerInfo->rtc.iceCandidateType = 0;
	peerInfo->rtc.iceTransportPolicy=YangTransportAll;


	//peerInfo->rtc.iceUsingLocalIp = yangfalse;
	peerInfo->rtc.iceServerPort=3478;

	peerInfo->rtc.rtcSocketProtocol=Yang_Socket_Protocol_Udp;
	peerInfo->rtc.turnSocketProtocol=Yang_Socket_Protocol_Udp;

	peerInfo->rtc.rtcPort = 8000;
	peerInfo->rtc.rtcLocalPort = 16000;

	peerInfo->rtc.turnReqInterval = 50;
	peerInfo->rtc.maxTurnWaitTime = 1000;



	//yang_strcpy(peerInfo->rtc.rtcServerIP, "127.0.0.1");
	yang_strcpy(peerInfo->rtc.iceServerIP, "127.0.0.1");
	//yang_strcpy(peerInfo->rtc.iceLocalIP, "127.0.0.1");
	yang_strcpy(peerInfo->rtc.iceUserName, "metartc");
	yang_strcpy(peerInfo->rtc.icePassword, "metartc");
}

void yang_avinfo_initPeerInfo(YangPeerInfo* peerInfo,YangAVInfo* avinfo){
	peerInfo->familyType=avinfo->sys.familyType;

	//peerInfo->userId=avinfo->sys.userId;


	peerInfo->pushAudio.sample = avinfo->audio.sample;
	peerInfo->pushAudio.channel = avinfo->audio.channel;
	peerInfo->pushAudio.audioEncoderType = avinfo->audio.audioEncoderType;
	peerInfo->pushAudio.enableAudioFec = avinfo->audio.enableAudioFec;

	peerInfo->pushVideo.width = avinfo->video.outWidth;
	peerInfo->pushVideo.height = avinfo->video.outHeight;
	peerInfo->pushVideo.fps = avinfo->video.frame;
	peerInfo->pushVideo.videoEncoderType = avinfo->video.videoEncoderType;

	yang_memcpy(&peerInfo->rtc,&avinfo->rtc,sizeof(YangRtcInfo));
	peerInfo->rtc.isControlled=yangfalse;
}



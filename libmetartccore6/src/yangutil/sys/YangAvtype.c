//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/yangavctype.h>
#include <yangutil/yangavctype_H.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfotype.h>

#include <memory.h>

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
		memcpy(dst->payload, src->payload, src->nb);
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
	sys->mediaServer=Yang_Server_Srs;
	sys->transType = Yang_Webrtc;
	sys->enableMultCamera = 0;
	sys->enableDataServer = 0;

	sys->rtmpPort = 1935;
	sys->srtPort = 8080;
	sys->rtcPort = 1985;
	sys->rtcLocalPort = 16000;
	sys->httpPort = 8080;
	sys->dataPort = 9999;
	sys->enableLogFile = 0;
	sys->logLevel = 1;
	sys->logLevel = 1;
	sys->cameraCount = 3;

	strcpy(sys->cameraIndexs, "1");
	strcpy(sys->rtmpServerIP, "127.0.0.1");
	strcpy(sys->srtServerIP, "127.0.0.1");
	strcpy(sys->rtcServerIP, "127.0.0.1");
	strcpy(sys->httpServerIP, "127.0.0.1");
	strcpy(sys->dataServerIP, "127.0.0.1");
}

void yang_avinfo_initEnc(YangVideoEncInfo *enc) {
	enc->preset = 3;
	enc->level_idc = 31;
	enc->profile = 0;
	enc->keyint_max = 30;
	enc->enc_threads = 4;
	enc->gop=60;
	enc->createMeta=true;
}

void yang_avinfo_initRtc(YangRtcInfo *rtc) {
	rtc->sessionTimeout= 30*1000000;
	rtc->enableDatachannel=1;
	rtc->iceCandidateType = 0;
	rtc->enableAudioBuffer = 0;
	//rtc->videoQueueCount = 5;
	rtc->iceUsingLocalIp=0;
	rtc->iceServerPort=3478;
	strcpy(rtc->iceServerIP, "127.0.0.1");
	strcpy(rtc->iceLocalIP, "127.0.0.1");
	strcpy(rtc->iceUserName, "metartc");
	strcpy(rtc->icePassword, "metartc");
}

void yang_init_avinfo(YangAVInfo* avinfo){

	memset(&avinfo->video, 0, sizeof(YangVideoInfo));
	memset(&avinfo->audio, 0, sizeof(YangAudioInfo));
	memset(&avinfo->sys, 0, sizeof(YangSysInfo));
	memset(&avinfo->enc, 0, sizeof(YangVideoEncInfo));
	memset(&avinfo->rtc, 0, sizeof(YangRtcInfo));

	yang_avinfo_initAudio(&avinfo->audio);
	yang_avinfo_initVideo(&avinfo->video);
	yang_avinfo_initSys(&avinfo->sys);
	yang_avinfo_initEnc(&avinfo->enc);
	yang_avinfo_initRtc(&avinfo->rtc);
}

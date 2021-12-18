/*
 * YangAvinfo.cpp
 *
 *  Created on: 2021年10月2日
 *      Author: yang
 */
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsl.h>
#include <string>
#include <string.h>

class YangInitContext {
public:
	YangInitContext() {
	}

	~YangInitContext() {
	}

	void initVideo(YangVideoInfo *video) {
		video->width = 1280;
		video->height = 720;
		video->outWidth = 1280;
		video->outHeight = 720;
		video->rate = 2048;
		video->frame = 10;

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
	void initAudio(YangAudioInfo *audio) {
		audio->sample = 48000;
		audio->frameSize = 960;
		audio->channel = 2;
		audio->bitrate = 128;
		audio->usingMono = 0;
		audio->hasAec = 0;
		audio->echoPath = 10;
		audio->aecBufferFrames = 0;
		audio->hasAudioHeader = 0;
		audio->audioEncoderType = 3;
		audio->audioDecoderType = 3;
		audio->audioPlayType = 0;
		audio->audioCacheNum = 10;
		audio->audioCacheSize = 10;
		audio->audioPlayCacheNum = 10;

		audio->aIndex = -1;
		audio->aSubIndex = 0;
		//aSubIndex

	}
	void initSys(YangSysInfo *sys) {

		sys->transType = 2;
		sys->isMultCamera = 0;
		sys->usingDataServer = 0;

		sys->rtmpPort = 1935;
		sys->srtPort = 8080;
		sys->rtcPort = 1985;
		sys->rtcLocalPort = 16000;
		sys->httpPort = 8080;
		sys->dataPort = 9999;
		sys->hasLogFile = 0;
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
	void initEnc(YangVideoEncInfo *enc) {
		enc->preset = 3;
		enc->level_idc = 31;
		enc->profile = 0;
		enc->keyint_max = 30;
		enc->enc_threads = 4;
		enc->gop=60;
		enc->createMeta=true;

	}
	void initRtc(YangRtcInfo *rtc) {
		rtc->sendTwcc = 0;
		rtc->mixAvqueue = 1;
        rtc->audioQueueCount = 5;
        rtc->videoQueueCount = 5;
	}
};

YangContext::YangContext() {
	init();
	m_certificate=NULL;
}

YangContext::~YangContext() {
	yang_closeLogFile();
	yang_delete(m_certificate);
}
void YangContext::init(char *filename) {
	YangIni ini;
	ini.init(filename);
	ini.initAudio(&audio);
	ini.initVideo(&video);
	ini.initSys(&sys);
	ini.initEnc(&enc);
	ini.initRtc(&rtc);
	initExt(&ini);
}

void YangContext::init() {
	memset(&video, 0, sizeof(YangVideoInfo));
	memset(&audio, 0, sizeof(YangAudioInfo));
	memset(&sys, 0, sizeof(YangSysInfo));
	memset(&enc, 0, sizeof(YangVideoEncInfo));
	memset(&rtc, 0, sizeof(YangRtcInfo));
	YangInitContext ini;

	ini.initAudio(&audio);
	ini.initVideo(&video);
	ini.initSys(&sys);
	ini.initEnc(&enc);
	ini.initRtc(&rtc);
	initExt();
}

YangCertificate* YangContext::getCertificate(){
	if(m_certificate==NULL){
		m_certificate=new YangCertificateImpl();
		if(m_certificate->init()) yang_error("certificate init error!");
	}
	return m_certificate;
}
void YangContext::initExt() {

}
void YangContext::initExt(void *filename) {

}

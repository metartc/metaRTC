//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGUTIL_YANGAVINFOTYPE_H_
#define YANGUTIL_YANGAVINFOTYPE_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>


typedef struct YangAudioInfo {
	int32_t sample;
	int32_t frameSize;
	int32_t bitrate;
	int32_t channel;

	yangbool enableMono;
	yangbool enableAec;
	int32_t echoPath;

	yangbool enableAudioFec;

	int32_t aecBufferFrames;
	int32_t audioCacheSize;
	int32_t audioCacheNum;
	int32_t audioPlayCacheNum;

	yangbool enableAudioHeader;
	int32_t audioEncoderType;
	int32_t audioDecoderType;
	int32_t audioPlayType;

	int32_t aIndex;
	int32_t aSubIndex;
}YangAudioInfo;

typedef struct YangVideoInfo {
	int32_t width; //= 800
	int32_t height; //= 600
	int32_t outWidth;
	int32_t outHeight;
	int32_t rate; // 512
	int32_t frame; //25
	int32_t rotate; // 16
	int32_t bitDepth;

	int32_t videoCacheNum;
	int32_t evideoCacheNum;
	int32_t videoPlayCacheNum;

	YangColorSpace videoCaptureFormat;
	YangColorSpace videoEncoderFormat;
	YangColorSpace videoDecoderFormat;

	int32_t videoEncoderType;
	int32_t videoDecoderType;
	int32_t videoEncHwType;
	int32_t videoDecHwType;
	int32_t vIndex;
}YangVideoInfo;

typedef struct YangVideoEncInfo {
	int32_t preset;
	int32_t level_idc;
	int32_t profile;
	int32_t keyint_max;
	int32_t enc_threads;
	int32_t gop;
	yangbool createMeta;
}YangVideoEncInfo;

typedef struct YangSysInfo {
	YangIpFamilyType familyType;
	yangbool enableLogFile;
	int32_t mediaServer;
	int32_t httpPort;
	int32_t transType;
	int32_t logLevel;
	char whipUrl[128];
	char whepUrl[128];
}YangSysInfo;

typedef struct YangRtcInfo {
	yangbool enableHttpServerSdp;

	int32_t sessionTimeout;
        //yangbool enableDatachannel;
	int32_t iceCandidateType;
	yangbool iceUsingLocalIp;
	int32_t iceServerPort;
	yangbool enableAudioBuffer;
	int32_t rtcSocketProtocol;
	int32_t turnSocketProtocol;

	int32_t rtcPort;
	int32_t rtcLocalPort;

	char localIp[32];
	char rtcServerIP[32];

	char iceServerIP[64];
	char iceLocalIP[64];
	char iceUserName[32];
	char icePassword[64];
}YangRtcInfo;

typedef struct YangAVInfo{
	YangSysInfo sys;
	YangAudioInfo audio;
	YangVideoInfo video;
	YangVideoEncInfo enc;
	YangRtcInfo rtc;
}YangAVInfo;


#ifdef __cplusplus
#include <yangstream/YangStreamManager.h>
#include <yangstream/YangSynBufferManager.h>
class YangContext {
public:
	YangContext();
	virtual ~YangContext();
	void init(char *filename);
	void init();

	virtual void initExt(void *filename);
	virtual void initExt();
public:

	YangAVInfo avinfo;
	YangRtcCallback rtcCallback;
	YangSendRtcMessage sendRtcMessage;

#if Yang_OS_ANDROID
	void* nativeWindow;
#endif

        YangSynBufferManager synMgr;
        YangStreamManager* streams;
};
extern "C"{
void yang_init_avinfo(YangAVInfo* avinfo);
}
#else
void yang_init_avinfo(YangAVInfo* avinfo);
#endif

#endif /* YANGUTIL_YANGTYPE_H_ */

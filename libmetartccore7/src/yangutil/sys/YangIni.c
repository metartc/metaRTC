//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangIni.h>

#if !(Yang_OS_ANDROID || Yang_OS_IOS)
#ifdef _MSC_VER
#include <direct.h>
#endif
#define SECTION_MAX_LEN 256
#define STRVALUE_MAX_LEN 256
#define LINE_CONTENT_MAX_LEN 256
#define FILEPATH_MAX 80

void yangTrim(char *ps, int32_t len) {
	for (int32_t i = 0; i < len; i++)
		if (*(ps + i) == 0x0a || *(ps + i) == 0x0d)
			*(ps + i) = 0x00;
}




int32_t yang_ini_IniReadValue(char* filename,const char *section, const char *key, char *val) {
	FILE *fp;
	int32_t i = 0;
	int32_t lineContentLen = 0;
	int32_t position = 0;
	int32_t ret = 1;
	char lineContent[LINE_CONTENT_MAX_LEN];
	yangbool bFoundSection = yangfalse;
	yangbool bFoundKey = yangfalse;
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("%s: Opent file %s failed.\n", __FILE__, filename);
		return -1;
	}
	int32_t stLen=0;
	while (feof(fp) == 0) {
		yang_memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
		if(!fgets(lineContent, LINE_CONTENT_MAX_LEN, fp)) continue;
		if ((lineContent[0] == ';') || (lineContent[0] == '\0')	|| (lineContent[0] == '\r') || (lineContent[0] == '\n')) {
			continue;
		}

		//check section
		if (yang_strncmp(lineContent, section, yang_strlen(section)) == 0) {
			bFoundSection = yangtrue;

			while (feof(fp) == 0) {
				yang_memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
				if(!fgets(lineContent, LINE_CONTENT_MAX_LEN, fp)) continue;
				//check key
				if (yang_strncmp(lineContent, key, yang_strlen(key)) == 0) {
					bFoundKey = yangtrue;
					lineContentLen = yang_strlen(lineContent);
					//find value
					for (i = yang_strlen(key); i < lineContentLen; i++) {
						if (lineContent[i] == '=') {
							position = i + 1;
							break;
						}
					}
					if (i >= lineContentLen)		break;
					char* tmp=lineContent + position;
					stLen=yang_strlen(tmp);
					//yang_strncpy(val, lineContent + position,stLen);
					yang_memcpy(val, lineContent + position,stLen);
					lineContentLen = yang_strlen(val);
					for (i = 0; i < lineContentLen; i++) {
						if ((lineContent[i] == '\0') || (lineContent[i] == '\r')
								|| (lineContent[i] == '\n')) {
							val[i] = '\0';
							break;
						}
					}
				} else if (lineContent[0] == '[') {
					break;
				}
			}
			break;
		}
	}
	if (!bFoundSection) {
		ret = -1;
	}
	else if (!bFoundKey) {
		ret = -1;
	}
	fclose(fp);
	yangTrim(val, yang_strlen(val));
	return ret;
}

int32_t yang_ini_readStringValue(char* filename,const char *section, const char *key,char *val, const char *p_defaultStr) {
	char sect[SECTION_MAX_LEN];

	if (section == NULL || key == NULL || val == NULL || filename == NULL) {
		printf("%s: input parameter(s) is NULL!\n", __func__);
		yang_strcpy(val, p_defaultStr);
		return -1;
	}

	yang_memset(sect, 0, SECTION_MAX_LEN);
	yang_sprintf(sect, "[%s]", section);
	int32_t ret = yang_ini_IniReadValue(filename,sect, key, val);
	if (ret == -1)
		yang_strcpy(val, p_defaultStr);
	return ret;
}

int32_t yang_ini_readStringValue1(char* filename,const char *section, const char *key,	char *val) {
	char sect[SECTION_MAX_LEN];
	if (section == NULL || key == NULL || val == NULL || filename == NULL) {
		printf("%s: input parameter(s) is NULL!\n", __func__);
		return -1;
	}

	yang_memset(sect, 0, SECTION_MAX_LEN);
	yang_sprintf(sect, "[%s]", section);
	return yang_ini_IniReadValue(filename,sect, key, val);
}

int32_t yang_ini_readIntValue(char* filename,const char *section, const char *key,	int32_t p_defaultInt) {
	char strValue[STRVALUE_MAX_LEN];
	yang_memset(strValue, '\0', STRVALUE_MAX_LEN);
	if (yang_ini_readStringValue1(filename,section, key, strValue) != 1) {
		return p_defaultInt;
	}
	return (atoi(strValue));
}

void yang_ini_initVideo(char* filename,YangVideoInfo* video){
	yang_memset(video,0,sizeof(YangVideoInfo));
	video->width = yang_ini_readIntValue(filename,"video", "width", 1280);
	video->height = yang_ini_readIntValue(filename,"video", "height", 720);
	video->outWidth = yang_ini_readIntValue(filename,"video", "outWidth", 1280);
	video->outHeight = yang_ini_readIntValue(filename,"video", "outHeight", 720);
	video->rate = yang_ini_readIntValue(filename,"video", "rate", 2048);
	video->frame = yang_ini_readIntValue(filename,"video", "frame", 10);
	video->rotate = yang_ini_readIntValue(filename,"video", "rotate", 0);
	video->bitDepth = yang_ini_readIntValue(filename,"video", "bitDepth", 8);

	video->videoCacheNum = yang_ini_readIntValue(filename,"video", "videoCacheNum", 50);
	video->evideoCacheNum = yang_ini_readIntValue(filename,"video", "evideoCacheNum", 50);
	video->videoPlayCacheNum = yang_ini_readIntValue(filename,"video", "videoPlayCacheNum", 5);

	video->videoCaptureFormat = (YangColorSpace)yang_ini_readIntValue(filename,"video", "videoCaptureFormat", YangYuy2);
	video->videoEncoderFormat = (YangColorSpace)yang_ini_readIntValue(filename,"video", "videoEncoderFormat", YangI420);
	video->videoDecoderFormat = (YangColorSpace)yang_ini_readIntValue(filename,"video", "videoDecoderFormat", YangI420);

	video->videoEncoderType = yang_ini_readIntValue(filename,"video", "videoEncoderType", 0);
	video->videoDecoderType = yang_ini_readIntValue(filename,"video", "videoDecoderType", 0);
	video->videoEncHwType = yang_ini_readIntValue(filename,"video", "videoEncHwType", 0);
	video->videoDecHwType = yang_ini_readIntValue(filename,"video", "videoDecHwType", 0);
	video->vIndex = yang_ini_readIntValue(filename,"video", "vIndex", 0);


}
void yang_ini_initAudio(char* filename,YangAudioInfo* audio){
	yang_memset(audio,0,sizeof(YangAudioInfo));
	audio->sample=yang_ini_readIntValue(filename,"audio", "sample", 48000);
	audio->frameSize=yang_ini_readIntValue(filename,"audio", "frameSize", 960);
	audio->channel=yang_ini_readIntValue(filename,"audio", "channel", 2);
	audio->bitrate = yang_ini_readIntValue(filename,"audio", "bitrate", 128);
	audio->enableMono=yang_ini_readIntValue(filename,"audio", "enableMono", yangfalse);
	audio->enableAec = yang_ini_readIntValue(filename,"audio", "enableAec", yangfalse);
	audio->enableAudioFec = yang_ini_readIntValue(filename,"audio", "enableAudioFec", yangtrue);

	audio->echoPath = yang_ini_readIntValue(filename,"audio", "echoPath", 10);
	audio->aecBufferFrames=yang_ini_readIntValue(filename,"audio", "aecBufferFrames", 0);
	audio->enableAudioHeader = yang_ini_readIntValue(filename,"audio", "enableAudioHeader", 0);
	audio->audioEncoderType = yang_ini_readIntValue(filename,"audio", "audioEncoderType", Yang_AED_OPUS);
	audio->audioDecoderType = yang_ini_readIntValue(filename,"audio", "audioDecoderType", Yang_AED_OPUS);
	audio->audioPlayType = yang_ini_readIntValue(filename,"audio", "audioPlayType", 0);
	audio->audioCacheNum = yang_ini_readIntValue(filename,"audio", "audioCacheNum", 100);
	audio->audioCacheSize = yang_ini_readIntValue(filename,"audio", "audioCacheSize", 100);
	audio->audioPlayCacheNum = yang_ini_readIntValue(filename,"audio", "audioPlayCacheNum", 10);

	audio->aIndex = yang_ini_readIntValue(filename,"audio", "aIndex", 0);
	audio->aSubIndex = yang_ini_readIntValue(filename,"audio", "aSubIndex", 0);
	//aSubIndex
	if(audio->enableMono){
		if(audio->audioEncoderType<2) audio->audioEncoderType=2;
		if(audio->audioDecoderType<2)audio->audioDecoderType=2;
		audio->channel=1;
		audio->sample=16000;
		audio->frameSize=320;
	}
}
void yang_ini_initSys(char* filename,YangSysInfo *sys){
	yang_memset(sys,0,sizeof(YangSysInfo));

	sys->familyType = (YangIpFamilyType)yang_ini_readIntValue(filename,"sys", "familyType", Yang_IpFamilyType_IPV4);
	sys->enableLogFile=yang_ini_readIntValue(filename,"sys", "enableLogFile", yangtrue);

	sys->transType = yang_ini_readIntValue(filename,"sys", "transType", Yang_Webrtc);
	sys->mediaServer = yang_ini_readIntValue(filename,"sys", "mediaServer", 0);
	sys->httpPort = yang_ini_readIntValue(filename,"sys", "httpPort", 1988);
	sys->logLevel = yang_ini_readIntValue(filename,"sys", "logLevel", 1);
	yang_ini_readStringValue(filename,"sys", "whipUrl", sys->whipUrl, "rtc/v1/whip/?app=%s&stream=%s");
	yang_ini_readStringValue(filename,"sys", "whepUrl", sys->whepUrl, "rtc/v1/whip-play/?app=%s&stream=%s");
}

void yang_ini_initEnc(char* filename,YangVideoEncInfo *enc){
	yang_memset(enc,0,sizeof(YangVideoEncInfo));
	enc->preset = yang_ini_readIntValue(filename,"enc", "preset", 3);
	enc->level_idc = yang_ini_readIntValue(filename,"enc", "level_idc", 31);
	enc->profile = yang_ini_readIntValue(filename,"enc", "profile", 0);
	enc->keyint_max = yang_ini_readIntValue(filename,"enc", "keyint_max", 30);
	enc->enc_threads = yang_ini_readIntValue(filename,"enc", "enc_threads", 4);
	enc->createMeta = yang_ini_readIntValue(filename,"enc", "createMeta", 1)==0?yangfalse:yangtrue;
	enc->gop = yang_ini_readIntValue(filename,"enc", "gop", 30);

}

void yang_ini_initRtc(char* filename,YangRtcInfo *rtc){
	yang_memset(rtc,0,sizeof(YangRtcInfo));
	rtc->enableHttpServerSdp = yang_ini_readIntValue(filename,"rtc", "enableHttpServerSdp", yangfalse);
	rtc->sessionTimeout = yang_ini_readIntValue(filename,"rtc", "sessionTimeout",  30*1000000);

	rtc->iceCandidateType = yang_ini_readIntValue(filename,"rtc", "iceCandidateType", 0);
	rtc->iceUsingLocalIp = yang_ini_readIntValue(filename,"rtc", "iceUsingLocalIp", 0);
	rtc->iceServerPort = yang_ini_readIntValue(filename,"rtc", "iceServerPort", 3478);
	rtc->enableAudioBuffer = yang_ini_readIntValue(filename,"rtc", "enableAudioBuffer", 0);

	rtc->rtcSocketProtocol = yang_ini_readIntValue(filename,"rtc", "rtcSocketProtocol", Yang_Socket_Protocol_Udp);
	rtc->turnSocketProtocol = yang_ini_readIntValue(filename,"rtc", "turnSocketProtocol",Yang_Socket_Protocol_Udp);

	rtc->rtcPort = yang_ini_readIntValue(filename,"rtc", "rtcPort", 1985);
	rtc->rtcLocalPort = yang_ini_readIntValue(filename,"rtc", "rtcLocalPort", 16000);

	yang_ini_readStringValue(filename,"rtc", "rtcServerIP", rtc->rtcServerIP, "127.0.0.1");
	yang_ini_readStringValue(filename,"rtc", "iceServerIP", rtc->iceServerIP, "127.0.0.1");
	yang_ini_readStringValue(filename,"rtc", "iceLocalIP", rtc->iceLocalIP, "127.0.0.1");
	yang_ini_readStringValue(filename,"rtc", "iceUserName", rtc->iceUserName, "metartc");
	yang_ini_readStringValue(filename,"rtc", "icePassword", rtc->icePassword, "metartc");
}

void yang_ini_initAvinfo(char* filename,YangAVInfo* avinfo){
	yang_ini_initAudio(filename,&avinfo->audio);
	yang_ini_initVideo(filename,&avinfo->video);
	yang_ini_initSys(filename,&avinfo->sys);
	yang_ini_initEnc(filename,&avinfo->enc);
	yang_ini_initRtc(filename,&avinfo->rtc);

}

void yang_create_ini(YangIni *ini, const char *p_filename) {
	char file1[300];
	yang_memset(file1, 0, 300);
	char file_path_getcwd[255];
	yang_memset(file_path_getcwd, 0, 255);
#ifdef _MSC_VER
	if(_getcwd(file_path_getcwd, 255)){
#else
		if (getcwd(file_path_getcwd, 255)) {
#endif
			yang_sprintf(file1, "%s/%s", file_path_getcwd, p_filename);
			yang_create_ini2(ini,file1);
		}

	}

	void yang_create_ini2(YangIni *ini, char *p_filename) {
		int32_t len = yang_strlen(p_filename) + 1;
		ini->filename = (char*) yang_malloc(len);
		yang_memset(ini->filename, 0, len);
		yang_memcpy(ini->filename, p_filename, len - 1);
		ini->initAudio = yang_ini_initAudio;
		ini->initVideo = yang_ini_initVideo;
		ini->initEnc = yang_ini_initEnc;
		ini->initRtc = yang_ini_initRtc;
		ini->initSys = yang_ini_initSys;

		ini->initAvinfo = yang_ini_initAvinfo;
		ini->readStringValue = yang_ini_readStringValue;
		ini->readIntValue = yang_ini_readIntValue;
	}

	void yang_destroy_ini(YangIni *ini) {
		yang_free(ini->filename);
	}

#endif

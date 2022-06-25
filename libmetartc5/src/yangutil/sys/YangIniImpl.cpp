//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangIni.h>
#include <yangutil/yang_unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <memory.h>
#include <stdlib.h>
#ifndef __ANDROID__
#ifdef _MSC_VER
#include <direct.h>
#endif
#define SECTION_MAX_LEN 256
#define STRVALUE_MAX_LEN 256
#define LINE_CONTENT_MAX_LEN 256
#define FILEPATH_MAX 80


YangIni::YangIni() {
	m_file = NULL;
}

YangIni::~YangIni() {
	yang_free(m_file);
}
void YangIni::init(const char *p_filename) {
	if(!p_filename) return;
	char file1[300];
	memset(file1, 0, 300);
	char file_path_getcwd[255];
	memset(file_path_getcwd, 0, 255);
#ifdef _MSC_VER
    if(_getcwd(file_path_getcwd, 255)){
#else
    if(getcwd(file_path_getcwd, 255)){
#endif
		sprintf(file1, "%s/%s", file_path_getcwd, p_filename);
		int32_t len = strlen(file1) + 1;
		m_file = (char*) malloc(len);
		memset(m_file, 0, len);
		strcpy(m_file, file1);
	}

}

void yangTrim(char *ps, int32_t len) {
	for (int32_t i = 0; i < len; i++)
		if (*(ps + i) == 0x0a || *(ps + i) == 0x0d)
			*(ps + i) = 0x00;
}
int32_t YangIni::IniReadValue(const char *section, const char *key, char *val) {
	FILE *fp;
	int32_t i = 0;
	int32_t lineContentLen = 0;
	int32_t position = 0;
	int32_t ret = 1;
	char lineContent[LINE_CONTENT_MAX_LEN];
	bool bFoundSection = false;
	bool bFoundKey = false;
	fp = fopen(m_file, "r");
	if (fp == NULL) {
		printf("%s: Opent file %s failed.\n", __FILE__, m_file);
		return -1;
	}
	int32_t stLen=0;
	while (feof(fp) == 0) {
		memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
		if(!fgets(lineContent, LINE_CONTENT_MAX_LEN, fp)) continue;
		if ((lineContent[0] == ';') || (lineContent[0] == '\0')	|| (lineContent[0] == '\r') || (lineContent[0] == '\n')) {
			continue;
		}

		//check section
		if (strncmp(lineContent, section, strlen(section)) == 0) {
			bFoundSection = true;
			//printf("Found section = %s\n", lineContent);
			while (feof(fp) == 0) {
				memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
				if(!fgets(lineContent, LINE_CONTENT_MAX_LEN, fp)) continue;
				//check key
				if (strncmp(lineContent, key, strlen(key)) == 0) {
					bFoundKey = true;
					lineContentLen = strlen(lineContent);
					//find value
					for (i = strlen(key); i < lineContentLen; i++) {
						if (lineContent[i] == '=') {
							position = i + 1;
							break;
						}
					}
					if (i >= lineContentLen)		break;

					stLen=strlen((char*)(lineContent + position));
					strncpy(val, lineContent + position,stLen);
					lineContentLen = strlen(val);
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
	yangTrim(val, strlen(val));
	return ret;
}

int32_t YangIni::readStringValue(const char *section, const char *key,char *val, const char *p_defaultStr) {
	char sect[SECTION_MAX_LEN];

	if (section == NULL || key == NULL || val == NULL || m_file == NULL) {
		printf("%s: input parameter(s) is NULL!\n", __func__);
		strcpy(val, p_defaultStr);
		return -1;
	}

	memset(sect, 0, SECTION_MAX_LEN);
	sprintf(sect, "[%s]", section);
	int32_t ret = IniReadValue(sect, key, val);
	if (ret == -1)
		strcpy(val, p_defaultStr);
	return ret;
}

int32_t YangIni::readStringValue1(const char *section, const char *key,	char *val) {
	char sect[SECTION_MAX_LEN];
	if (section == NULL || key == NULL || val == NULL || m_file == NULL) {
		printf("%s: input parameter(s) is NULL!\n", __func__);
		return -1;
	}

	memset(sect, 0, SECTION_MAX_LEN);
	sprintf(sect, "[%s]", section);
	return IniReadValue(sect, key, val);
}

int32_t YangIni::readIntValue(const char *section, const char *key,	int32_t p_defaultInt) {
	char strValue[STRVALUE_MAX_LEN];
	memset(strValue, '\0', STRVALUE_MAX_LEN);
	if (readStringValue1(section, key, strValue) != 1) {
		return p_defaultInt;
	}
	return (atoi(strValue));
}


void YangIni::initVideo(YangVideoInfo* video){
	memset(video,0,sizeof(YangVideoInfo));
	video->width = readIntValue("video", "width", 1280);
		video->height = readIntValue("video", "height", 720);
		video->outWidth = readIntValue("video", "outWidth", 1280);
		video->outHeight = readIntValue("video", "outHeight", 720);
		video->rate = readIntValue("video", "rate", 2048);
		video->frame = readIntValue("video", "frame", 10);
		video->rotate = readIntValue("video", "rotate", 0);
		video->bitDepth = readIntValue("video", "bitDepth", 8);

		video->videoCacheNum = readIntValue("video", "videoCacheNum", 50);
		video->evideoCacheNum = readIntValue("video", "evideoCacheNum", 50);
		video->videoPlayCacheNum = readIntValue("video", "videoPlayCacheNum", 5);

		video->videoCaptureFormat = (YangYuvType)readIntValue("video", "videoCaptureFormat", YangYuy2);
		video->videoEncoderFormat = (YangYuvType)readIntValue("video", "videoEncoderFormat", YangI420);
		video->videoDecoderFormat = (YangYuvType)readIntValue("video", "videoDecoderFormat", YangI420);

		video->videoEncoderType = readIntValue("video", "videoEncoderType", 0);
		video->videoDecoderType = readIntValue("video", "videoDecoderType", 0);
		video->videoEncHwType = readIntValue("video", "videoEncHwType", 0);
		video->videoDecHwType = readIntValue("video", "videoDecHwType", 0);
		video->vIndex = readIntValue("video", "vIndex", 0);


}
       void YangIni::initAudio(YangAudioInfo* audio){
    		memset(audio,0,sizeof(YangAudioInfo));
    		audio->sample=readIntValue("audio", "sample", 44100);
    		audio->frameSize=readIntValue("audio", "frameSize", 1024);
    		audio->channel=readIntValue("audio", "channel", 2);
    		audio->bitrate = readIntValue("audio", "bitrate", 128);
    		audio->usingMono=readIntValue("audio", "usingMono", 0);
    		audio->hasAec = readIntValue("audio", "hasAec", 0);
    		audio->echoPath = readIntValue("audio", "echoPath", 10);
    		audio->aecBufferFrames=readIntValue("audio", "aecBufferFrames", 0);
    		audio->hasAudioHeader = readIntValue("audio", "hasAudioHeader", 0);
    		audio->audioEncoderType = readIntValue("audio", "audioEncoderType", 0);
    		audio->audioDecoderType = readIntValue("audio", "audioDecoderType", 0);
    		audio->audioPlayType = readIntValue("audio", "audioPlayType", 0);
       		audio->audioCacheNum = readIntValue("audio", "audioCacheNum", 100);
    		audio->audioCacheSize = readIntValue("audio", "audioCacheSize", 100);
    		audio->audioPlayCacheNum = readIntValue("audio", "audioPlayCacheNum", 10);

    		audio->aIndex = readIntValue("audio", "aIndex", 0);
    		audio->aSubIndex = readIntValue("audio", "aSubIndex", 0);
    		//aSubIndex
    		if(audio->usingMono){
    			if(audio->audioEncoderType<2) audio->audioEncoderType=2;
    			if(audio->audioDecoderType<2)audio->audioDecoderType=2;
    			audio->channel=1;
    			audio->sample=16000;
    			audio->frameSize=320;
    		}
       }
       void YangIni::initSys(YangSysInfo *sys){
    	   memset(sys,0,sizeof(YangSysInfo));

		   sys->transType = readIntValue("sys", "transType", 0);
		   sys->mediaServer = readIntValue("sys", "mediaServer", 0);
    	   sys->isMultCamera = readIntValue("sys", "isMultCamera", 0);
    	   sys->usingDataServer = readIntValue("sys", "usingDataServer", 0);

    		sys->rtmpPort = readIntValue("sys", "rtmpPort", 1935);
    		sys->srtPort = readIntValue("sys", "srtPort", 8080);
    		sys->rtcPort = readIntValue("sys", "rtcPort", 1985);
    		sys->rtcLocalPort = readIntValue("sys", "rtcLocalPort", 16000);
    		sys->httpPort = readIntValue("sys", "httpPort", 8080);
    		sys->dataPort = readIntValue("sys", "dataPort", 9999);
    		sys->hasLogFile=readIntValue("sys", "hasLogFile", 0);
    		sys->logLevel = readIntValue("sys", "logLevel", 1);
    		sys->logLevel = readIntValue("sys", "logLevel", 1);
    		sys->cameraCount = readIntValue("sys", "cameraCount", 3);
    		readStringValue("sys", "cameraIndexs", sys->cameraIndexs, "1");
    		readStringValue("sys", "rtmpServerIP", sys->rtmpServerIP, "127.0.0.1");
    		readStringValue("sys", "srtServerIP", sys->srtServerIP, "127.0.0.1");
    		readStringValue("sys", "rtcServerIP", sys->rtcServerIP, "127.0.0.1");
    		readStringValue("sys", "httpServerIP", sys->httpServerIP, "127.0.0.1");
    		readStringValue("sys", "dataServerIP", sys->dataServerIP, "127.0.0.1");
       }

       void YangIni::initEnc(YangVideoEncInfo *enc){
    	   memset(enc,0,sizeof(YangVideoEncInfo));
    	   enc->preset = readIntValue("enc", "preset", 3);
    	   enc->level_idc = readIntValue("enc", "level_idc", 31);
    	   enc->profile = readIntValue("enc", "profile", 0);
    	   enc->keyint_max = readIntValue("enc", "keyint_max", 30);
    	   enc->enc_threads = readIntValue("enc", "enc_threads", 4);
    	   enc->createMeta = readIntValue("enc", "createMeta", 1)==0?false:true;
    	   enc->gop = readIntValue("enc", "gop", 30);

       }

       void YangIni::initRtc(YangRtcInfo *rtc){
           memset(rtc,0,sizeof(YangRtcInfo));
           rtc->sessionTimeout = readIntValue("rtc", "sessionTimeout",  30*1000000);
		   rtc->usingDatachannel = readIntValue("rtc", "usingDatachannel", 1);
           rtc->hasIceServer = readIntValue("rtc", "hasIceServer", 0);
           rtc->iceStunPort = readIntValue("rtc", "iceStunPort", 3478);
           rtc->audioQueueCount = readIntValue("rtc", "audioQueueCount", 5);
           rtc->videoQueueCount = readIntValue("rtc", "videoQueueCount", 5);

           readStringValue("rtc", "iceServerIP", rtc->iceServerIP, "127.0.0.1");

       }

#endif

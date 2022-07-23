//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrecord/YangRecordMp4.h>
#include <yangencoder/YangAudioEncoderMeta.h>
#include <yangutil/sys/YangEndian.h>
#include "yangutil/sys/YangLog.h"
#include "time.h"
#include "yangutil/yang_unistd.h"
YangRecordMp4::YangRecordMp4(YangAudioInfo *paudio,YangVideoInfo *pvideo,YangVideoEncInfo *penc) {
	m_audio=paudio;
	m_video=pvideo;
	m_enc=penc;
	m_isConvert = 1;
	m_isStart=0;
	videoDestLen = 0;
	frameType = 1;

	m_videoTimestamp = 0;

	m_preVideotimestamp = 0;
	m_mp4Stamp = 0;
	oldalltick = 0, alltick = 0;
	isMp4 = 1;
	curVideoTimestamp = 0;
	m_startStamp=0;
	m_prePauseTime=0,m_afterPauseTime=0;
	m_pauseTime=0;
	m_fileId=0;
	m_alltime=0;
	m_alltime1=0;
	m_fileTimeLen=3600;
	m_isCreateNewFile=0;
	memset(&m_mp4Para,0,sizeof(m_mp4Para));
	srcVideoSource=NULL;
	srcAudioSource=NULL;
	m_in_audioBuffer=NULL;
	m_in_videoBuffer=NULL;
	mp4 = NULL;
	flv = NULL;
	memset(&m_audioFrame,0,sizeof(YangFrame));
	memset(&m_videoFrame,0,sizeof(YangFrame));

}

YangRecordMp4::~YangRecordMp4(void) {
	m_audio=NULL;
		m_video=NULL;
		m_enc=NULL;
	yang_free(m_mp4Para.fileName);
	m_mp4Para.vmd=NULL;
	m_in_audioBuffer=NULL;
	m_in_videoBuffer=NULL;
	yang_deleteA(srcVideoSource);
	yang_deleteA(srcAudioSource);
	yang_delete(mp4);
	yang_delete(flv);
}
void YangRecordMp4::stop() {
	stopLoop();
}

void YangRecordMp4::run() {
	m_isStart=1;
	startLoop();
	m_isStart=0;
}
void YangRecordMp4::setInVideoBuffer(YangVideoEncoderBuffer *pbuf) {
	m_in_videoBuffer = pbuf;
}
void YangRecordMp4::setInAudioBuffer(YangAudioEncoderBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}


void YangRecordMp4::writeAudioData() {

	m_in_audioBuffer->getAudio(&m_audioFrame);

	if (m_audioFrame.nb < 0 || m_audioFrame.nb  > 1000)	return;
	if (isMp4)
		mp4->WriteAudioPacket(&m_audioFrame);
	else
		flv->WriteAudioPacket(&m_audioFrame);
}


void YangRecordMp4::writeVideoData() {
	m_videoFrame.payload=srcVideoSource+4;
	m_in_videoBuffer->getEVideo(&m_videoFrame);

	m_videoTimestamp=m_videoFrame.pts;
	if(m_alltime>m_fileTimeLen){
		m_isCreateNewFile=1;
		m_alltime=0;
		m_alltime1=0;
	}
	if(m_isCreateNewFile&&frameType){

		createNewfile();
		m_isCreateNewFile=0;
		m_alltime=0;
		m_alltime1=0;
	}
	if (m_startStamp == 0)		m_startStamp = m_videoTimestamp;

	if (isMp4) {

		alltick = (m_videoTimestamp - m_startStamp-m_pauseTime) * 9 / 100;

		m_mp4Stamp = alltick - oldalltick;
		m_alltime1+=m_mp4Stamp;
		m_alltime=m_alltime1/90000;

		yang_put_be32((char*)srcVideoSource,m_videoFrame.nb);
		m_videoFrame.payload=srcVideoSource;
		m_videoFrame.nb+=4;
		m_videoFrame.pts=m_mp4Stamp;

		mp4->WriteVideoPacket(&m_videoFrame);
		oldalltick=alltick;
	} else {
		curVideoTimestamp = (m_videoTimestamp - m_startStamp-m_pauseTime) / 1000;
		m_alltime=curVideoTimestamp/1000;
		m_videoFrame.payload=srcVideoSource;
		m_videoFrame.pts=curVideoTimestamp;

		flv->WriteVideoPacket(&m_videoFrame);
	}

}


void YangRecordMp4::pauseRec(){
	m_prePauseTime=yang_get_milli_tick();
}
	void YangRecordMp4::resumeRec(){

		m_afterPauseTime=yang_get_milli_tick();//m_time.getMilliTick();
		m_pauseTime+=m_afterPauseTime-m_prePauseTime;

	}
	void YangRecordMp4::initRecPara(){
		m_preVideotimestamp = 0;
		m_mp4Stamp = 0;
		oldalltick = 0, alltick = 0;
		curVideoTimestamp = 0;
		m_startStamp=0;
		m_prePauseTime=0,m_afterPauseTime=0;
		m_pauseTime=0;
		//if(m_in_audioBuffer) m_in_audioBuffer->resetIndex();
		//if(m_in_videoBuffer) m_in_videoBuffer->resetIndex();
	}
	void YangRecordMp4::createNewfile(){
		closeRec();
		char filename1[256];
		memset(filename1,0,256);
		memcpy(filename1,m_mp4Para.fileName,strlen(m_mp4Para.fileName)-4);
		m_fileId++;
		char filename[300];
		memset(filename,0,300);
		sprintf(filename,"%s_%d.%s",filename1,m_fileId,isMp4?"mp4":"flv");
		createFile(filename);
		initRecPara();

	}
	void YangRecordMp4::createFile(char* filename){
		if (isMp4) {
				mp4 = new YangMp4File(filename, m_video);
				mp4->init(m_mp4Para.vmd->mp4Meta.sps, m_mp4Para.vmd->mp4Meta.spsLen);
				uint8_t pasc[1024];
				int32_t pasclen=0;
				YangAudioEncoderMeta yem;
				yem.createMeta(pasc,&pasclen);
				memset(m_mp4Para.asc,0,10);
				memcpy(m_mp4Para.asc,pasc,pasclen);
				m_mp4Para.ascLen=pasclen;
				mp4->WriteAudioInfo(pasc,pasclen,NULL, 0);
				mp4->WriteVideoInfo(m_mp4Para.vmd->mp4Meta.vps, m_mp4Para.vmd->mp4Meta.vpsLen,m_mp4Para.vmd->mp4Meta.sps, m_mp4Para.vmd->mp4Meta.spsLen,
						m_mp4Para.vmd->mp4Meta.pps, m_mp4Para.vmd->mp4Meta.ppsLen);
			} else {
				flv = new YangFlvWriter(filename, m_audio,m_video);
				flv->framerate = m_video->frame;
				flv->i_bitrate = (double)m_video->rate;
				flv->i_level_idc = 31;
				flv->Start();

				flv->WriteVideoInfo(m_mp4Para.vmd->livingMeta.buffer, m_mp4Para.vmd->livingMeta.bufLen);
				//flv->WriteAudioHeadPacket();

			}

	}
void YangRecordMp4::initPara(YangVideoMeta *p_vmd,char *filename, int32_t p_isMp4) {
	isMp4 = p_isMp4;
	m_mp4Para.vmd=p_vmd;
	int32_t flen=strlen(filename);
	m_mp4Para.fileName=(char*)malloc(flen+1);
	memset(m_mp4Para.fileName,0,flen+1);
	memcpy(m_mp4Para.fileName,filename,flen);
	createFile(filename);

}
void YangRecordMp4::setFileTimeLen(int32_t ptlen_min){
	m_fileTimeLen=ptlen_min*60;
}
void YangRecordMp4::closeRec() {
	if (mp4 != NULL) {
		mp4->closeMp4();

		yang_delete(mp4);
	}
	if (flv != NULL) {
		flv->Close();

		yang_delete(flv);
	}

}


void YangRecordMp4::startLoop() {
	m_isConvert = 1;

	if(srcVideoSource==NULL) srcVideoSource=new uint8_t[512*1024];
	if(srcAudioSource==NULL) srcAudioSource=new	uint8_t[2048];
	m_audioFrame.payload=srcAudioSource;
	m_videoFrame.payload=srcVideoSource;
	srcVideoSource[0]=0x00;
	srcVideoSource[1]=0x00;
	srcVideoSource[2]=0x00;
	srcVideoSource[3]=0x01;
	while (m_isConvert == 1) {
		if (m_in_videoBuffer->size() > 5) {
			yang_info("write cache big..%d\n", m_in_videoBuffer->size());
		}
		if (m_in_videoBuffer->size() ==0 &&  m_in_audioBuffer->size() ==0) {

			yang_usleep(2000);
			continue;
		}


			if(m_in_audioBuffer->size()>1) writeAudioData();
			if(m_in_videoBuffer->size()){
				writeVideoData();

			}

	}
	//while (m_in_videoBuffer->size()> 1 || m_in_audioBuffer->size() > 0) {
	//	writeAudioData();
		//writeVideoData();
	//}

	closeRec();

}
void YangRecordMp4::stopLoop() {
	m_isConvert = 0;
}



/*
 * YangRecord.cpp
 *
 *  Created on: 2020年10月11日
 *      Author: yang
 */

#include <yangrecord/YangRecord.h>
#include <yangencoder/YangAudioEncoderMeta.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/yang_unistd.h>
#include <time.h>
#include <memory.h>

YangRecord::YangRecord(YangAudioInfo *paudio,YangVideoInfo *pvideo,YangVideoEncInfo *penc) {
	m_audio=paudio;
	m_video=pvideo;
	m_enc=penc;
	videoDestLen = 0;
	preVideotimestamp = 0;
	minusStamp = 0;
	oldalltick = 0, alltick = 0;
	isMp4 = 1;
	curVideoTimestamp = 0;
	basestamp=0;
	m_prePauseTime=0,m_afterPauseTime=0;
	m_pauseTime=0;
	m_fileId=0;
	m_alltime=0;
	m_alltime1=0;
	m_fileTimeLen=3600;
	m_isCreateNewFile=0;
	memset(&m_mp4Para,0,sizeof(m_mp4Para));
	mp4 = NULL;
	flv = NULL;

}

YangRecord::~YangRecord(void) {
	m_audio=NULL;
	m_video=NULL;
	m_enc=NULL;
	yang_free(m_mp4Para.fileName);
	m_mp4Para.vmd=NULL;
	yang_delete(mp4);
	yang_delete(flv);
}


void YangRecord::writeAudioData(YangFrame* audioFrame) {
	if (audioFrame->nb < 0 || audioFrame->nb > 1000)	return;
	if (isMp4){
		mp4->WriteAudioPacket(audioFrame);
	}
	else
		flv->WriteAudioPacket(audioFrame);
}


void YangRecord::writeVideoData(YangFrame* videoFrame) {
	if(m_alltime>m_fileTimeLen){
		m_isCreateNewFile=1;
		m_alltime=0;
		m_alltime1=0;
	}
	if(m_isCreateNewFile&&videoFrame->frametype){
	//	int64_t t1=m_time.getMilliTick();
		createNewfile();
	//	int64_t t2=m_time.getMilliTick()-t1;
	//	printf("\n**************************differ==================%lld",t2);
		m_isCreateNewFile=0;
		m_alltime=0;
		m_alltime1=0;
	}
	if (basestamp == 0)		basestamp = videoFrame->timestamp;
	if (isMp4) {
		alltick = (videoFrame->timestamp - basestamp-m_pauseTime) * 9 / 100;
		//if(m_pauseTime) printf("%lld-%lld-%lld,",videoTimestamp,basestamp,m_pauseTime);
		minusStamp = alltick - oldalltick;
		m_alltime1+=minusStamp;
		m_alltime=m_alltime1/90000;
		//printf("v%lld,",videoTimestamp);
		yang_put_be32((char*)videoFrame->payload,videoFrame->nb);
		videoFrame->nb+=4;
		videoFrame->timestamp=minusStamp;
		mp4->WriteVideoPacket(videoFrame);
		//oldalltick += minusStamp;
		oldalltick=alltick;
	} else {
		curVideoTimestamp = (videoFrame->timestamp - basestamp-m_pauseTime) / 1000;
		m_alltime=curVideoTimestamp/1000;
		//curVideoTimestamp = (videoTimestamp - basestamp-m_pauseTime) / 100;
		videoFrame->timestamp=curVideoTimestamp;
		flv->WriteVideoPacket(videoFrame);
	}

}


void YangRecord::pauseRec(){
	m_prePauseTime=yang_get_milli_tick();//m_time.getMilliTick();
}
	void YangRecord::resumeRec(){

		m_afterPauseTime=yang_get_milli_tick();//m_time.getMilliTick();
		m_pauseTime+=m_afterPauseTime-m_prePauseTime;
		//printf("")
	}
	void YangRecord::initRecPara(){
		//videoTimestamp = 0;
		//videoBufLen = 0;
		preVideotimestamp = 0;
		minusStamp = 0;
		oldalltick = 0, alltick = 0;
		curVideoTimestamp = 0;
		basestamp=0;
		m_prePauseTime=0,m_afterPauseTime=0;
		m_pauseTime=0;
		//if(m_in_audioBuffer) m_in_audioBuffer->resetIndex();
		//if(m_in_videoBuffer) m_in_videoBuffer->resetIndex();
	}
	void YangRecord::createNewfile(){
		closeRec();
		char filename1[128];
		memset(filename1,0,128);
		memcpy(filename1,m_mp4Para.fileName,strlen(m_mp4Para.fileName)-4);
		m_fileId++;
		char filename[255];
		memset(filename,0,255);
		sprintf(filename,"%s_%d.%s",filename1,m_fileId,isMp4?"mp4":"flv");
		createFile(filename);
		initRecPara();

	}
	void YangRecord::createFile(char* filename){
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
				//printf("\n*************************aselen==%d\n",pasclen);
				//for(int32_t i=0;i<pasclen;i++) printf("%x,",pasc[i]);
				//printf("\n");
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
void YangRecord::initPara(YangVideoMeta *p_vmd,char *filename, int32_t p_isMp4) {
	//printf("\n*************filename===%s",filename);
	isMp4 = p_isMp4;
	m_mp4Para.vmd=p_vmd;
	int32_t flen=strlen(filename);
	m_mp4Para.fileName=(char*)malloc(flen+1);
	memset(m_mp4Para.fileName,0,flen+1);
	memcpy(m_mp4Para.fileName,filename,flen);
	createFile(filename);

}
void YangRecord::setFileTimeLen(int32_t ptlen_min){
	m_fileTimeLen=ptlen_min*60;
}
void YangRecord::closeRec() {
	if (mp4 != NULL) {
		mp4->closeMp4();
		printf("................mp4 is closed!\n");
		yang_delete(mp4);
	}
	if (flv != NULL) {
		flv->Close();
		printf("................flv is closed!\n");
		yang_delete(flv);
	}

}





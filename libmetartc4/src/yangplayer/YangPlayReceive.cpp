//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangplayer/YangPlayReceive.h>
#include <yangutil/yang_unistd.h>
#include <yangplayer/YangPlayerBase.h>

#include <yangavutil/video/YangNalu.h>
#include <yangstream/YangStreamHandle.h>
#include <yangutil/sys/YangLog.h>

void g_playrecv_receiveAudio(void* user,YangFrame *audioFrame){
	if(user==NULL) return;
	YangPlayReceive* rtcHandle=(YangPlayReceive*)user;
	rtcHandle->receiveAudio(audioFrame);
}
void g_playrecv_receiveVideo(void* user,YangFrame *videoFrame){
	if(user==NULL) return;

	YangPlayReceive* rtcHandle=(YangPlayReceive*)user;
	rtcHandle->receiveVideo(videoFrame);
}


YangPlayReceive::YangPlayReceive(YangContext* pcontext) {
	m_context=pcontext;
	m_isStart = 0;
	m_out_videoBuffer = NULL;
	m_out_audioBuffer = NULL;
	isReceived = 0;
	isReceiveConvert = 0;
	m_headLen = 1; //pcontext->audio.audioDecoderType == 0 ? 2 : 1;
	m_recv = NULL;
	m_recvCallback.receiveAudio=g_playrecv_receiveAudio;
	m_recvCallback.receiveVideo=g_playrecv_receiveVideo;
	m_recvCallback.context=this;
}

YangPlayReceive::~YangPlayReceive() {

	disConnect();


	if (m_recv)
		delete m_recv;
	m_recv = NULL;

	m_out_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
	m_context=NULL;
}

void YangPlayReceive::disConnect() {
    if (m_isStart) {
        stop();
        while (m_isStart) {
            yang_usleep(1000);
        }
    }
    if(m_recv) {
        m_recv->disConnectServer(m_recv->context);
        yang_destroy_streamHandle(m_recv);
        yang_free(m_recv);
    }

}
void YangPlayReceive::setBuffer(YangAudioEncoderBuffer *al,	YangVideoDecoderBuffer *vl) {
	m_out_audioBuffer = al;
	m_out_videoBuffer = vl;
}
void YangPlayReceive::receiveAudio(YangFrame *audioFrame) {
	audioFrame->payload += m_headLen;
	audioFrame->nb -= m_headLen;
	m_out_audioBuffer->putPlayAudio(audioFrame);
}
void YangPlayReceive::receiveVideo(YangFrame *videoFrame) {
    if(videoFrame==NULL||videoFrame->payload==NULL) return;
    uint8_t *temp = videoFrame->payload;
    int videoLen=videoFrame->nb;

    if( (temp[0] == 0x27|| temp[0] == 0x2c)&&temp[1] == 0x01){
        videoFrame->payload = temp + 5;
        videoFrame->nb -= 5;
        videoFrame->frametype = YANG_Frametype_P;
        if(yang_hasH264Pframe(videoFrame->payload)) m_out_videoBuffer->putEVideo(videoFrame);
        return;
    }
    if ((temp[0] == 0x17 || temp[0] == 0x1c) ) {
        if(temp[1] == 0x00){
            videoFrame->frametype = YANG_Frametype_Spspps;
            m_out_videoBuffer->putEVideo(videoFrame);
            return;
        }
        if(temp[1] == 0x01){

            YangH264NaluData nalu;
            videoFrame->payload=temp+5;
            videoFrame->nb=videoLen-5;
            videoFrame->frametype =YANG_Frametype_I;
           m_out_videoBuffer->putEVideo(videoFrame);
           /** yang_parseH264Nalu(videoFrame,&nalu);

            if(nalu.spsppsPos>-1){
                uint8_t meta[200] = { 0 };
                videoFrame->payload=meta;
                yang_getH264SpsppseNalu(videoFrame,temp+5+nalu.spsppsPos);
                videoFrame->frametype = YANG_Frametype_Spspps;
                m_out_videoBuffer->putEVideo(videoFrame);
            }
            if(nalu.keyframePos>-1){
                videoFrame->payload = temp + 5+nalu.keyframePos;
                videoFrame->nb = videoLen-5-nalu.keyframePos;
                videoFrame->frametype =YANG_Frametype_I;
                m_out_videoBuffer->putEVideo(videoFrame);

            }**/
        }
    }
}

int32_t YangPlayReceive::init(int32_t nettype, char* server, int32_t pport,char* app,char* stream) {
	if (!m_recv){
		m_recv = (YangStreamHandle*)calloc(sizeof(YangStreamHandle),1);
		YangStreamConfig streamConf;
			memset(&streamConf,0,sizeof(streamConf));


        //	m_recv->context->videoStream = 1;
        //	m_recv->context->uid = 0;
			m_context->avinfo.sys.transType=nettype;



			streamConf.localPort = m_context->avinfo.sys.rtcLocalPort;
			strcpy(streamConf.remoteIp,server);
			streamConf.remotePort = pport;
			strcpy(streamConf.app,app);
			strcpy(streamConf.stream,stream);
			streamConf.uid = 0;
			streamConf.streamOptType = Yang_Stream_Play;

			streamConf.recvCallback.context=this;
			streamConf.recvCallback.receiveAudio=g_playrecv_receiveAudio;
			streamConf.recvCallback.receiveVideo=g_playrecv_receiveVideo;

			yang_create_streamHandle(m_recv, &streamConf,&m_context->avinfo);
	}

	int32_t ret = m_recv->connectServer(m_recv->context);
	if (ret)
		return yang_error_wrap(ret,"rtmp connect fail!");

	m_recv->context->streamInit = 1;
	return ret;

}
void YangPlayReceive::stop() {
	isReceiveConvert = 0;
}
void YangPlayReceive::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangPlayReceive::startLoop() {

	yang_reindex(m_out_audioBuffer);
	yang_reindex(m_out_videoBuffer);

	int32_t bufLen = 0;
	int32_t retCode = Yang_Ok;
	isReceiveConvert = 1;
	isReceived = 1;

	while (isReceiveConvert == 1) {
		if (!m_recv) {
			yang_usleep(2000);
			continue;
		}

		if (!m_recv->context->streamInit)	continue;
		bufLen = 0;

		retCode = m_recv->receiveData(m_recv->context,&bufLen);

		if (bufLen == 0)
			yang_usleep(2000);
	}	            		//end while

	isReceived = 0;
}

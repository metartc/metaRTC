#include <yangutil/yang_unistd.h>
#include <yangplayer/YangPlayerBase.h>
#include <yangplayer/YangPlayReceive.h>
#include <yangavutil/video/YangNalu.h>
#include <yangutil/sys/YangLog.h>
#include <yangstream/YangStreamFactory.h>


YangPlayReceive::YangPlayReceive(YangContext* pcontext) {
	m_context=pcontext;
	m_isStart = 0;
	m_out_videoBuffer = NULL;
	m_out_audioBuffer = NULL;
	isReceived = 0;
	isReceiveConvert = 0;
	m_headLen = 1; //pcontext->audio.audioDecoderType == 0 ? 2 : 1;
	m_recv = NULL;
}

YangPlayReceive::~YangPlayReceive() {
	disConnect();
	if (isReceiveConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}

	if (m_recv)
		delete m_recv;
	m_recv = NULL;

	m_out_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
	m_context=NULL;
}

void YangPlayReceive::disConnect() {
    if(m_recv) m_recv->disConnectServer();
    yang_delete(m_recv);

}
void YangPlayReceive::setBuffer(YangAudioEncoderBuffer *al,
		YangVideoDecoderBuffer *vl) {
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

            yang_parseH264Nalu(videoFrame,&nalu);

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

            }
        }
    }
}

int32_t YangPlayReceive::init(int32_t nettype, string server, int32_t pport,
		string stream) {
	//m_play=
	YangStreamFactory sf;
	if (!m_recv)
		m_recv = sf.createStreamHandle(nettype, 0,m_context);
	YangStreamConfig streamConf;
	streamConf.app = "live";
	streamConf.streamOptType = Yang_Stream_Play;
	streamConf.uid = 0;

	streamConf.localIp = "127.0.0.1";
	streamConf.localPort = 8100;
	streamConf.serverIp = server;
	streamConf.serverPort = pport;
	streamConf.stream = stream;
	m_recv->init(&streamConf);
	m_recv->m_videoStream = 1;
	m_recv->setReceiveCallback(this);
	m_recv->m_uid = 0;
	int32_t ret = m_recv->connectServer();
	if (ret)
		return ret;
	m_recv->m_streamInit = 1;
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
			yang_usleep(10000);
			continue;
		}

		if (!m_recv->m_streamInit)
			continue;
		bufLen = 0;
		//tuid=m_players.at(i)->m_uid;
		retCode = m_recv->receiveData(&bufLen);


		//if (retCode) {
		//	yang_error("Receive Data Error:%d", retCode);
		//	break;
		//}
		if (bufLen == 0)
			yang_usleep(2000);
	}	            		//end while

	isReceived = 0;
}

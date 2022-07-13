//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "YangIpc.h"
#include <yangp2p/YangP2pCommon.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangMath.h>
void g_ipc_receiveData(void* context,YangFrame* msgFrame){
	YangIpc* win=(YangIpc*)context;

    //win->setRecvText((char*)msgFrame->payload,msgFrame->nb);
}
YangIpc::YangIpc() {
    m_context=new YangContext();
    m_context->init((char*)"yang_config.ini");
    m_context->avinfo.video.videoEncoderFormat=YangI420;
#if Yang_Using_Openh264
    m_context->avinfo.enc.createMeta=0;
#else
    m_context->avinfo.enc.createMeta=1;
#endif
    init();
       yang_setLogLevle(m_context->avinfo.sys.logLevel);
       yang_setLogFile(m_context->avinfo.sys.hasLogFile);

       m_context->avinfo.sys.httpPort=1988;
       m_context->avinfo.sys.mediaServer=Yang_Server_P2p;//Yang_Server_Srs/Yang_Server_Zlm/Yang_Server_P2p
       m_context->avinfo.sys.rtcLocalPort=10000+yang_random()%15000;
       memset(m_context->avinfo.sys.localIp,0,sizeof(m_context->avinfo.sys.localIp));
       yang_getLocalInfo(m_context->avinfo.sys.localIp);
       m_hasAudio=true;

         //using h264 h265
         m_context->avinfo.video.videoEncoderType=Yang_VED_264;//Yang_VED_265;


         m_context->streams.m_streamState=this;
         m_context->avinfo.audio.hasAec=1;

         m_context->channeldataRecv.context=this;
         m_context->channeldataRecv.receiveData=g_ipc_receiveData;

         m_context->avinfo.rtc.usingDatachannel=1;

         strcpy(m_context->avinfo.rtc.iceServerIP,"182.92.163.143");
         m_context->avinfo.rtc.iceStunPort=3478;
         m_context->avinfo.rtc.hasIceServer=0;
}

YangIpc::~YangIpc() {
	   yang_delete(m_message);
	   yang_delete(m_context);
}

void YangIpc::init(){
	  m_context->avinfo.audio.usingMono=0;
	    m_context->avinfo.audio.sample=48000;
	    m_context->avinfo.audio.channel=2;
	    m_context->avinfo.audio.hasAec=1;
	    m_context->avinfo.audio.audioCacheNum=8;
	    m_context->avinfo.audio.audioCacheSize=8;
	    m_context->avinfo.audio.audioPlayCacheNum=8;

	    m_context->avinfo.video.videoCacheNum=10;
	    m_context->avinfo.video.evideoCacheNum=10;
	    m_context->avinfo.video.videoPlayCacheNum=10;

	    m_context->avinfo.audio.audioEncoderType=Yang_AED_OPUS;
	    m_context->avinfo.sys.rtcLocalPort=17000;
	    m_context->avinfo.enc.enc_threads=4;
}
void YangIpc::initPreview(){
        yang_post_message(YangM_Push_StartVideoCapture,0,NULL);
}

void YangIpc::streamStateNotify(int32_t puid,YangStreamOptType opt,bool isConnect){
    yang_trace("online user play count==%d,push count==%d",m_context->streams.getPlayOnlineCount(),m_context->streams.getPushOnlineCount());
   // if(m_context){
        //m_context->streams.getPlayOnlineCount()>0?true:false;
   // }
}
void YangIpc::success(){

}

void YangIpc::failure(int32_t errcode){
    //on_m_b_play_clicked();
   yang_error("push error(%d)",errcode);

}

void YangIpc::receiveSysMessage(YangSysMessage *mss, int32_t err){
    switch (mss->messageId) {
    case YangM_P2p_Connect:
        {
            if(err){
            	 yang_error("push error(%d)",err);
             }
        }
        break;
   case YangM_P2p_Disconnect:
        break;
    case YangM_P2p_Play_Start:
         break;
    case YangM_P2p_Play_Stop:
         break;
   case YangM_P2p_StartVideoCapture:
    {
       //m_videoBuffer=m_p2pfactory.getPreVideoBuffer(m_message);
         break;
     }

    }


}

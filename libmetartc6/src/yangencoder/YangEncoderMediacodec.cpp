//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangencoder/YangEncoderMediacodec.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangavutil/video/YangNalu.h>

#ifdef __ANDROID__
#include <EGL/egl.h>
YangEncoderMediacodec::YangEncoderMediacodec() {
	m_sendKeyframe=0;
	m_colorSpace=19;
	m_width=640;
	m_height=480;
	m_yuvLen=640*480*3/2;
	m_hasHeader=false;
	m_mediaCodec=NULL;
	metaInit=false;
	metaLen=0;
	m_sps.nb=0;
	m_sps.bytes=NULL;
	m_pps.nb=0;
	m_pps.bytes=NULL;
}
YangEncoderMediacodec::~YangEncoderMediacodec(void) {
	if(m_mediaCodec){
		AMediaCodec_flush(m_mediaCodec);
		AMediaCodec_stop(m_mediaCodec);
		AMediaCodec_delete(m_mediaCodec);
		m_mediaCodec=NULL;
	}
	yang_free(m_sps.bytes);
	yang_free(m_pps.bytes);
}
void YangEncoderMediacodec::sendMsgToEncoder(YangRtcEncoderMessage *msg){
	if(msg->request==Yang_Req_Sendkeyframe){
		m_sendKeyframe=1;
	}else if(msg->request==Yang_Req_HighLostPacketRate){

	}else if(msg->request==Yang_Req_LowLostPacketRate){

	}
}

void YangEncoderMediacodec::setVideoMetaData(YangVideoMeta *pvmd) {

}

int32_t YangEncoderMediacodec::init(YangContext* pcontext,YangVideoInfo* videoInfo) {
	if (m_isInit == 1)		return Yang_Ok;

	YangVideoEncInfo* encInfo=&pcontext->avinfo.enc;
	setVideoPara(videoInfo, encInfo);
	m_width=videoInfo->outWidth;
	m_height=videoInfo->outHeight;
	m_yuvLen=m_width*m_height*3/2;
	if(videoInfo->videoEncoderFormat==YangI420){
		m_colorSpace=19;
	}else if(videoInfo->videoEncoderFormat==YangYv12){
		m_colorSpace=20;
	}else if(videoInfo->videoEncoderFormat==YangNv12){
		m_colorSpace=21;
	}else if(videoInfo->videoEncoderFormat==YangNv21){
		m_colorSpace=39;
	}

	AMediaFormat *format ;
	m_mediaCodec = AMediaCodec_createEncoderByType("video/avc");//h264 // 创建 codec 编码器
	if(m_mediaCodec == NULL){
		yang_error("createEncoder erro[%s%d]\n",__FUNCTION__ ,__LINE__);
	}
	format = AMediaFormat_new();

	AMediaFormat_setString(format, "mime", "video/avc");//h265:video/hevc h264:video/avc
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_WIDTH,m_width);
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_HEIGHT,m_height);
	//在OMX_IVCommon.h https://www.androidos.net.cn/android/9.0.0_r8/xref/frameworks/native/headers/media_plugin/media/openmax/OMX_IVCommon.h
	//AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_COLOR_FORMAT,OMX_COLOR_FormatYUV420Planar);
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_COLOR_FORMAT,m_colorSpace); //i42p=19 yv12=20 nv12=21  NV21 = 39
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_FRAME_RATE,videoInfo->frame);
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_BIT_RATE,videoInfo->rate*1000);
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_I_FRAME_INTERVAL,1);

	yang_trace("fromat:%s\n",AMediaFormat_toString(format));

	//这里配置 format
	media_status_t status = AMediaCodec_configure(m_mediaCodec,format,NULL,NULL,AMEDIACODEC_CONFIGURE_FLAG_ENCODE);//解码，flags 给0，编码给AMEDIACODEC_CONFIGURE_FLAG_ENCODE
	if(status!=0){
		yang_trace("erro config %d\n",status);
		return ERROR_CODEC_Encode;
	}

	AMediaFormat_delete(format);


	if(pcontext->nativeWindow){
		//ANativeWindow *surface=(ANativeWindow *)pcontext->nativeWindow;
		//AMediaCodec_createInputSurface(m_mediaCodec, &surface);
		/*if (mEglCore == NULL)
		        mEglCore = new EglCore(eglGetCurrentContext(), FLAG_TRY_GLES2|FLAG_RECORDABLE);
		    if (mWindowSurface == NULL)
		        mWindowSurface = new WindowSurface(mEglCore, mWindowRef, true);*/

	}

	if ((status = AMediaCodec_start(m_mediaCodec)) != AMEDIA_OK) {
		yang_error("AMediaCodec_start: Could not start encoder.");
		return ERROR_CODEC_Encode;
	}
	yang_trace("AMediaCodec_start: encoder successfully started");

	//AMediaFormat *format2 = AMediaCodec_getOutputFormat(m_mediaCodec);
	m_isInit = 1;

	return Yang_Ok;

}

int32_t YangEncoderMediacodec::encode(YangFrame* pframe, YangEncoderCallback* pcallback) {
	if(pframe==NULL||pframe->payload==NULL) return ERROR_CODEC_Encode;
	bool isKeyFrame = false;
	media_status_t status ;
	if (m_sendKeyframe == 1) {
		m_sendKeyframe = 2;
		AMediaFormat *format = AMediaFormat_new();
		//AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_I_FRAME_INTERVAL,1);
		AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_IS_SYNC_FRAME,0);
		status=AMediaCodec_setParameters(m_mediaCodec,format);
		/*if(status==AMEDIA_OK){
			yang_trace("set parameter AMEDIAFORMAT_KEY_IS_SYNC_FRAME success!");
		}else{
			yang_error("set parameter AMEDIAFORMAT_KEY_IS_SYNC_FRAME success fail=%d",status);
		}*/
		AMediaFormat_delete(format);
	}

	//请求buffer
	ssize_t bufidx = AMediaCodec_dequeueInputBuffer(m_mediaCodec, 0);
	if (bufidx >= 0) {
		size_t bufsize;
		uint8_t *buf = AMediaCodec_getInputBuffer(m_mediaCodec, bufidx,	&bufsize);
		memcpy(buf,pframe->payload,m_yuvLen);
		AMediaCodec_queueInputBuffer(m_mediaCodec, bufidx, 0, m_yuvLen, pframe->pts,	0);
	}



	//取输出buffer
	ssize_t outindex;

	do{
		AMediaCodecBufferInfo info;
		outindex = AMediaCodec_dequeueOutputBuffer(m_mediaCodec, &info, -1);
		if (outindex >= 0) {
			//在这里取走编码后的数据
			//释放buffer给编码器
			size_t outsize;
			uint8_t *buf = AMediaCodec_getOutputBuffer(m_mediaCodec, outindex,	&outsize);

			if(!metaInit){
				if(info.flags == 2){
					if((buf[4] & kNalTypeMask) == YangAvcNaluTypeSPS){
						uint8_t* sps=buf+4;
						int32_t pps_pos=yang_getNalupos(sps,info.size-4);
						uint8_t* pps=sps+pps_pos;
						int32_t spsLen=pps-sps-4;
						int32_t ppsLen=info.size-spsLen-8;
						m_sps.nb=spsLen+4;
						m_sps.bytes=(char*)malloc(spsLen+4);
						yang_put_be32(m_sps.bytes,spsLen);
						memcpy(m_sps.bytes+4,sps,spsLen);

						m_pps.nb=ppsLen+4;
						m_pps.bytes=(char*)malloc(ppsLen+4);
						yang_put_be32(m_pps.bytes,ppsLen);
						memcpy(m_pps.bytes+4,pps,ppsLen);
						metaLen=spsLen+ppsLen+8;
						metaInit=true;

					}
					AMediaCodec_releaseOutputBuffer(m_mediaCodec, outindex, false);

				}
				continue;
			}
			if(info.flags == 1)
				pframe->frametype=YANG_Frametype_I;
			else
				pframe->frametype=YANG_Frametype_P;



			if(pframe->frametype==YANG_Frametype_I){
				memcpy(m_vbuffer,m_sps.bytes,m_sps.nb);
				memcpy(m_vbuffer+m_sps.nb,m_pps.bytes,m_pps.nb);
				memcpy(m_vbuffer + metaLen , buf,	info.size);
				yang_put_be32((char*) (m_vbuffer + metaLen),	(uint32_t) info.size);

				pframe->nb=info.size+metaLen;
				pframe->payload=m_vbuffer;

			}else{
				pframe->payload = buf+4;
				pframe->nb = info.size-4;

			}

			if (pcallback)	pcallback->onVideoData(pframe);
			AMediaCodec_releaseOutputBuffer(m_mediaCodec, outindex, false);
		}
	}while(outindex<0);
	if (m_sendKeyframe == 2) {
		//X264_TYPE_AUTO;
		m_sendKeyframe = 0;
		yang_trace("\nsendkey.frametype=%d\n",	pframe->frametype);
	}

	return Yang_Ok;
}
#endif


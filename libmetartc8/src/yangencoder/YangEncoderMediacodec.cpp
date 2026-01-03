//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangencoder/YangEncoderMediacodec.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangvideo/YangNalu.h>

#if Yang_OS_ANDROID
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
}
YangEncoderMediacodec::~YangEncoderMediacodec(void) {
	if(m_mediaCodec){
		AMediaCodec_flush(m_mediaCodec);
		AMediaCodec_stop(m_mediaCodec);
		AMediaCodec_delete(m_mediaCodec);
		m_mediaCodec=NULL;
	}
}
void YangEncoderMediacodec::sendMsgToEncoder(YangRtcEncoderMessage *msg){
	if(msg->request==Yang_Req_Sendkeyframe){
		m_sendKeyframe=1;
	}else if(msg->request==Yang_Req_HighLostPacketRate){

	}else if(msg->request==Yang_Req_LowLostPacketRate){

	}
}


int32_t YangEncoderMediacodec::init(YangVideoInfo* videoInfo,YangVideoEncInfo* encInfo) {
	if (m_isInit == 1)		return Yang_Ok;

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
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_BIT_RATE,videoInfo->rate);

	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_BITRATE_MODE,2);//BITRATE_MODE_CQ 0 BITRATE_MODE_VBR 1 BITRATE_MODE_CBR 2
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_PROFILE,1);
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_LEVEL,0x200);//30 0x100 31 0x200 32 0x400
	AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_I_FRAME_INTERVAL,1);

	yang_trace("fromat:%s\n",AMediaFormat_toString(format));

	//这里配置 format
	media_status_t status = AMediaCodec_configure(m_mediaCodec,format,NULL,NULL,AMEDIACODEC_CONFIGURE_FLAG_ENCODE);//解码，flags 给0，编码给AMEDIACODEC_CONFIGURE_FLAG_ENCODE
	if(status!=0){
		yang_trace("erro config %d\n",status);
		return ERROR_CODEC_Encode;
	}

	AMediaFormat_delete(format);

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

		AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_IS_SYNC_FRAME,0);
		status=AMediaCodec_setParameters(m_mediaCodec,format);
		if(status==AMEDIA_OK){
				yang_trace("set parameter AMEDIAFORMAT_KEY_IS_SYNC_FRAME success!");
			}else{
				yang_error("set parameter AMEDIAFORMAT_KEY_IS_SYNC_FRAME success fail=%d",status);
			}
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
						memcpy(m_vbuffer,buf,info.size);
						metaLen=info.size;
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
				memcpy(m_vbuffer + metaLen , buf,	info.size);
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
		m_sendKeyframe = 0;
		yang_trace("\nsendkey.frametype=%d\n",	pframe->frametype);
	}

	return Yang_Ok;
}
#endif


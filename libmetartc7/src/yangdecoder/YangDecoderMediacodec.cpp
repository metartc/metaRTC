//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangdecoder/YangDecoderMediacodec.h>
#if Yang_OS_ANDROID
#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangMeta.h>
#include <yangavutil/video/YangSpspps.h>
#include <stdlib.h>


YangDecoderMediacodec::YangDecoderMediacodec(YangVideoInfo *pcontext,YangVideoCodec pencdectype) {
	m_encDecType= pencdectype;
	m_context=pcontext;
	m_bitDepth=pcontext->bitDepth;
	m_mediaCodec=NULL;
	tryagainCount=0;
	m_width = 0;
	m_height = 0;
	m_yuvLen=0;
	yLen = 0;
	uLen = 0;
	ret = 0;
	m_isNv12=yangfalse;
	m_nv12Buffer=NULL;
}
YangDecoderMediacodec::~YangDecoderMediacodec() {
	decode_close();
}

void YangDecoderMediacodec::parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid,
		int32_t *phei, int32_t *pfps) {

	uint8_t headers[128];
	memset(headers, 0, 128);
	int32_t headerLen = 0;
	if(m_encDecType==Yang_VED_H264){
		uint8_t* sps=NULL;
		int32_t spsLen=0;
		yang_h264_decode_getH264RtmpHeader(p,&sps,&spsLen);
		yang_h264_decode_spspps(sps,spsLen,pwid,phei,pfps);
	}else if(m_encDecType==Yang_VED_H265) {

	}
	m_width = *pwid;
	m_height= *phei ;


	YangSample sps2,sps;
	YangSample pps2,pps;
	yang_decodeMetaH264(p,pLen,&sps2,&pps2);
	sps.bytes=(char*)calloc(sps2.nb+4,1);
	pps.bytes=(char*)calloc(pps2.nb+4,1);

	sps.bytes[3]=0x01;
	pps.bytes[3]=0x01;
	memcpy(sps.bytes+4,sps2.bytes,sps2.nb);
	memcpy(pps.bytes+4,pps2.bytes,pps2.nb);
	sps.nb=sps2.nb+4;
	pps.nb=pps2.nb+4;

	int32_t bitLen=(m_bitDepth==8?1:2);

	yLen = m_width * m_height*bitLen;
	uLen = yLen / 4;
	m_yuvLen = yLen * 3 / 2;
	int32_t localColorFMT;
	AMediaFormat* format=NULL;
	const char* mine = "video/avc";
	m_mediaCodec =  AMediaCodec_createDecoderByType(mine);
	AMediaFormat* videoFormat = AMediaFormat_new();
	AMediaFormat_setString(videoFormat, "mime", "video/avc");
	AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_WIDTH, m_width); // 视频宽度
	AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_HEIGHT, m_height); // 视频高度
	AMediaFormat_setBuffer(videoFormat, "csd-0", sps.bytes, sps.nb); // sps
	AMediaFormat_setBuffer(videoFormat, "csd-1", pps.bytes, pps.nb); // pps
	//这里配置 format
	media_status_t status = AMediaCodec_configure(m_mediaCodec,videoFormat,NULL,NULL,0);//解码，flags 给0，编码给AMEDIACODEC_CONFIGURE_FLAG_ENCODE
	if(status!=0){
		AMediaFormat_delete(videoFormat);
		yang_trace("erro config %d\n",status);
		goto ret;
	}

	AMediaFormat_delete(videoFormat);
	AMediaCodec_start(m_mediaCodec);
	if(status!=0){
		yang_trace("start erro %d\n",status);
		goto ret;
	}
	format = AMediaCodec_getOutputFormat(m_mediaCodec);

	AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT, &localColorFMT);
	if(localColorFMT==21) {
		m_isNv12=yangtrue;
		if(m_nv12Buffer==NULL) m_nv12Buffer=new uint8_t[m_width*m_height*3/2];
	}
	yang_trace("output color fomat=%d",localColorFMT);
	AMediaFormat_delete(format);
	m_isInit = 1;
	ret:
	yang_free(sps.bytes);
	yang_free(pps.bytes);

}

void YangDecoderMediacodec::init() {


}

int32_t YangDecoderMediacodec::decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback){
	if(m_isInit==0) return yang_error_wrap(ERROR_CODEC_Decode,"mediacodec not ready");
	size_t outsize=0;
	ssize_t bufidx = AMediaCodec_dequeueInputBuffer(m_mediaCodec, 2000);
	if (bufidx >= 0) {
		// 获取buffer的索引
		uint8_t* inputBuf = AMediaCodec_getInputBuffer(m_mediaCodec, bufidx, &outsize);
		if (inputBuf != nullptr && videoFrame->nb <= outsize) {
			// 将待解码的数据copy到硬件中
			memcpy(inputBuf, videoFrame->payload, videoFrame->nb);
			media_status_t status = AMediaCodec_queueInputBuffer(m_mediaCodec, bufidx, 0, videoFrame->nb, videoFrame->pts, 0);
			if(AMEDIA_OK!=status){
				yang_trace("data errorid=%d",status);
			}
		}
	}
	AMediaCodecBufferInfo info;
	ssize_t outbufidx = AMediaCodec_dequeueOutputBuffer(m_mediaCodec, &info, 2000);
	if (outbufidx >= 0) {
		outsize=0;
		uint8_t* outputBuf = AMediaCodec_getOutputBuffer(m_mediaCodec, outbufidx, &outsize);
		if (outputBuf != nullptr) {

			videoFrame->nb=m_yuvLen;
			tryagainCount=0;
			if(m_isNv12){
						if(m_nv12Buffer==NULL) m_nv12Buffer=new uint8_t[m_width*m_height*3/2];
						m_yuv.nv12toI420(outputBuf, m_nv12Buffer, m_width, m_height);
						videoFrame->payload=m_nv12Buffer;
			}else{
						videoFrame->payload=outputBuf;
			}
			if(pcallback) pcallback->onVideoData(videoFrame);
			AMediaCodec_releaseOutputBuffer(m_mediaCodec, outbufidx, info.size != 0);
			return Yang_Ok;
		}
	}else{
		switch (outbufidx) {
		case AMEDIACODEC_INFO_TRY_AGAIN_LATER:
			yang_trace("android mediacodec decode error try again(%d)",tryagainCount);
			tryagainCount++;
			if(tryagainCount>20){
				//AMediaCodec_flush(m_mediaCodec);
				yang_error("android mediacodec decode error try again>40times ,consider fail");
			}
			return tryagainCount>10?2:1;
			break;
		case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED:
			// 解码输出的格式发生变化
		{
			auto format = AMediaCodec_getOutputFormat(m_mediaCodec);
			AMediaFormat_getInt32(format, "width", &m_width);
			AMediaFormat_getInt32(format, "height", &m_height);
			int32_t localColorFMT;
			AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT, &localColorFMT);
			if(localColorFMT==21) {
				m_isNv12=yangtrue;
				if(m_nv12Buffer==NULL) m_nv12Buffer=new uint8_t[m_width*m_height*3/2];
			}
			//localColorFMT 19 yuv420p 21 nv12
			int32_t stride = 0;
			AMediaFormat_getInt32(format, "stride", &stride);
			if(stride == 0) {
				stride = m_width;
			}
			tryagainCount=0;
			yang_trace("change>>>output color fomat=%d,width=%d,height=%d,stride=%d",localColorFMT,m_width,m_height,stride);

			break;
		}
		case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
			break;
		default:
			return ERROR_CODEC_Decode;
			break;
		}
	}

	return Yang_Ok;
}

void YangDecoderMediacodec::decode_close() {

	if(m_mediaCodec){
		//AMediaCodec_flush(m_mediaCodec);
		AMediaCodec_stop(m_mediaCodec);
		AMediaCodec_delete(m_mediaCodec);
		m_mediaCodec=NULL;
	}
	yang_deleteA(m_nv12Buffer);
}

#endif

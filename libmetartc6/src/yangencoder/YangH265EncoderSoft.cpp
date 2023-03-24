//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangencoder/YangH265EncoderSoft.h>
#include <yangutil/yangavh265.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>




#define HEX2BIN(a)      (((a)&0x40)?((a)&0xf)+9:((a)&0xf))

int32_t YangH265EncoderSoft::hex2bin(char *str, char **hex) {
	char *ptr;
	int32_t i, l = strlen(str);
	if (l & 1)
		return 0;
	*hex = (char*) malloc(l / 2);
	ptr = *hex;
	if (!ptr)
		return 0;
	for (i = 0; i < l; i += 2)
		*ptr++ = (HEX2BIN(str[i]) << 4) | HEX2BIN(str[i + 1]);
	return l / 2;
}




void YangH265EncoderSoft::loadLib(){
	yang_x265_param_default=(void (*)( x265_param * ))m_lib.loadFunction("x265_param_default");
	yang_x265_param_apply_profile=(int32_t (*)( x265_param *, const char *profile ))m_lib.loadFunction("x265_param_apply_profile");
	yang_x265_param_default_preset=(int32_t (*)(x265_param *, const char *preset, const char *tune))m_lib.loadFunction("x265_param_default_preset");
	char s[30];
	memset(s,0,30);
	sprintf(s,"x265_encoder_open_%d",X265_BUILD);
	yang_x265_param_alloc =(x265_param *(*)(void))m_lib.loadFunction("x265_param_alloc");
	yang_x265_param_free=(void (*)(x265_param *))m_lib.loadFunction("x265_param_free");
	yang_x265_encoder_open=(x265_encoder* (*)( x265_param * ))m_lib.loadFunction(s);
	yang_x265_picture_alloc=(x265_picture* (*)())m_lib.loadFunction("x265_picture_alloc");
	yang_x265_picture_init=(void (*)(x265_param *param, x265_picture *pic))m_lib.loadFunction("x265_picture_init");
	yang_x265_encoder_encode=(int32_t  (*)( x265_encoder *, x265_nal **pp_nal,uint32_t  *pi_nal, x265_picture *pic_in, x265_picture *pic_out ))m_lib.loadFunction("x265_encoder_encode");
	yang_x265_picture_free=(void (*)( x265_picture *pic ))m_lib.loadFunction("x265_picture_free");
	yang_x265_encoder_close=(void (*)( x265_encoder * ))m_lib.loadFunction("x265_encoder_close");
}
void YangH265EncoderSoft::unloadLib(){
	yang_x265_param_default_preset=NULL;
	yang_x265_param_alloc=NULL;
	yang_x265_param_free=NULL;
	yang_x265_param_default=NULL;
	yang_x265_param_apply_profile=NULL;
	yang_x265_picture_alloc=NULL;
	yang_x265_picture_init=NULL;
	yang_x265_encoder_encode=NULL;
	yang_x265_picture_free=NULL;
	yang_x265_encoder_close=NULL;
	yang_x265_encoder_open=NULL;
}
YangH265EncoderSoft::YangH265EncoderSoft() {
	m_nal = NULL;
	m_265Nal = NULL;
	m_265Pic = NULL;
	m_265Handle = NULL;
	nal_len = 0;
	destLength = 0;
	m_i265Nal = 0;
	nal_len = 0;
	m_frametype=0;
	m_hasHeader=false;
	m_buffer=NULL;
	unloadLib();
}

YangH265EncoderSoft::~YangH265EncoderSoft(void) {
	yang_free(m_buffer);
	m_265Pic->planes[0]=NULL;
	m_265Pic->planes[1]=NULL;
	m_265Pic->planes[2]=NULL;
	yang_x265_picture_free(m_265Pic);
	yang_x265_encoder_close(m_265Handle);
	m_265Pic = NULL;
	m_265Handle = NULL;
	m_265Nal = NULL;

	m_nal = NULL;

	unloadLib();
	m_lib.unloadObject();
}
void YangH265EncoderSoft::sendMsgToEncoder(YangRtcEncoderMessage *msg){
	if(msg->request==Yang_Req_Sendkeyframe){
		m_sendKeyframe=1;
	}else if(msg->request==Yang_Req_HighLostPacketRate){

	}else if(msg->request==Yang_Req_LowLostPacketRate){

	}
}
void YangH265EncoderSoft::setVideoMetaData(YangVideoMeta *pvmd) {

}
void YangH265EncoderSoft::initX265Param(YangVideoInfo *pvp,YangVideoEncInfo *penc,x265_param *param){

	param->sourceBitDepth=pvp->bitDepth;

	param->sourceWidth = pvp->outWidth;   //set frame width
	param->sourceHeight = pvp->outHeight;  //set frame height
	param->interlaceMode=0;

	param->bframes = 0;


	param->internalCsp=X265_CSP_I420;
	param->levelIdc = penc->level_idc;//31;

	if(penc->preset>4){
		param->rc.qpMin= 10;
		param->rc.qpMax = 30;
	}
	param->fpsNum = pvp->frame;  //30;
	param->fpsDenom=1;

	param->keyframeMax = pvp->frame * 3;
	param->logLevel = X265_LOG_NONE;


}
int32_t YangH265EncoderSoft::init(YangContext* pcontext,YangVideoInfo* videoInfo) {
	if (m_isInit == 1)
		return Yang_Ok;
	m_lib.loadObject("libx265");
	loadLib();

	YangVideoEncInfo* encInfo=&pcontext->avinfo.enc;
	setVideoPara(videoInfo,encInfo);
	x265_param *param = yang_x265_param_alloc();
	m_265Pic = new x265_picture();

	memset(m_265Pic, 0, sizeof(x265_picture));

	yang_x265_param_default(param);  //set default param
	if(encInfo->preset<5) yang_x265_param_default_preset(param,x265_preset_names[encInfo->preset],x265_tune_names[3]);
	initX265Param(videoInfo,encInfo,param);
	m_hasHeader=!encInfo->createMeta;
	param->bRepeatHeaders=m_hasHeader?1:0;
	int32_t profileIndex=0;
	if(videoInfo->bitDepth==10) profileIndex=1;
	if(videoInfo->bitDepth==12) profileIndex=12;

	yang_x265_param_apply_profile(param, x265_profile_names[profileIndex]);

	if ((m_265Handle = yang_x265_encoder_open(param)) == NULL) {
		yang_error("RE init x265_encoder_open failed");
		exit(0);
		//return yang_error_wrap(ERROR_CODEC_Encode,"RE init x265_encoder_open failed");
	}

	m_265Pic=yang_x265_picture_alloc();
	yang_x265_picture_init(param,m_265Pic);
	int32_t bitLen=param->sourceBitDepth==8?1:2;
	int32_t fileLen=bitLen*param->sourceWidth*param->sourceHeight*3/2;
	int32_t allLen=bitLen*param->sourceWidth*param->sourceHeight;
	m_buffer=(uint8_t*)malloc(fileLen);

	m_265Pic->planes[0]=m_buffer;
	m_265Pic->planes[1]=m_buffer+allLen;
	m_265Pic->planes[2]=m_buffer+allLen*5/4;
	m_265Pic->stride[0]=param->sourceWidth;
	m_265Pic->stride[1]=param->sourceWidth/2;
	m_265Pic->stride[2]=param->sourceWidth/2;

	m_isInit = 1;
	yang_x265_param_free(param);
	param = NULL;
	return Yang_Ok;

}

int32_t YangH265EncoderSoft::encode(YangFrame* pframe,  YangEncoderCallback* pcallback){

	memcpy(m_265Pic->planes[0], pframe->payload, pframe->nb);
	destLength = 0;
	int32_t isSlicekeyframe=0;
	int32_t isKeyFrame=0;
	m_frametype = YANG_Frametype_P;
	if (m_sendKeyframe == 1) {
		m_sendKeyframe = 2;
		m_265Pic->sliceType = X265_TYPE_IDR;
	}
	if (yang_x265_encoder_encode(m_265Handle, &m_265Nal, &m_i265Nal, m_265Pic,
			NULL) < 0) {
		return yang_error_wrap(ERROR_CODEC_Encode_Video, "x265_encoder_encode failed/n");
	}

	for (uint32_t i = 0; i < m_i265Nal; i++) {
		m_nal = m_265Nal[i].payload;
		if (m_265Nal[i].type >= YANG_NAL_UNIT_CODED_SLICE_BLA&&m_265Nal[i].type <= YANG_NAL_UNIT_CODED_SLICE_CRA){
			if(m_frametype==0) m_frametype = YANG_Frametype_I;
			isSlicekeyframe=1;
		}else if(m_hasHeader){
			if((m_265Nal[i].type >= YANG_NAL_UNIT_VPS&&m_265Nal[i].type <= YANG_NAL_UNIT_PPS)){
				if(m_frametype==0) m_frametype = YANG_Frametype_I;
				isSlicekeyframe=1;
			}else{
				isSlicekeyframe=0;
			}
		}

		if (m_hasHeader && m_frametype==YANG_Frametype_I) {
			if(!isSlicekeyframe) continue;
			isKeyFrame=(m_265Nal[i].type >= YANG_NAL_UNIT_CODED_SLICE_BLA&&m_265Nal[i].type <= YANG_NAL_UNIT_CODED_SLICE_CRA)?1:0;
			if(isKeyFrame){
				memcpy(m_vbuffer + destLength+1, m_nal, m_265Nal[i].sizeBytes);
				yang_put_be32((char*) (m_vbuffer + destLength),	(uint32_t) (m_265Nal[i].sizeBytes-3));
				destLength += m_265Nal[i].sizeBytes+1;
			}else{
				memcpy(m_vbuffer + destLength, m_nal, m_265Nal[i].sizeBytes);
				yang_put_be32((char*) (m_vbuffer + destLength),	(uint32_t) (m_265Nal[i].sizeBytes-4));
				destLength += m_265Nal[i].sizeBytes;
			}

		} else {
			memcpy(m_vbuffer + destLength, m_nal + 4, m_265Nal[i].sizeBytes - 4);
			destLength += (m_265Nal[i].sizeBytes - 4);
		}

	}

	pframe->payload = m_vbuffer;
	pframe->frametype = m_frametype;
	pframe->nb = destLength;

	if (pcallback)
		pcallback->onVideoData(pframe);
	if (m_sendKeyframe == 2) {
		m_265Pic->sliceType = X265_TYPE_AUTO;
		m_sendKeyframe = 0;
	}
	return Yang_Ok;
}



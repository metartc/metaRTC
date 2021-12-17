#include <yangavutil/video/YangMeta.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <memory.h>
void yang_createH264Meta(YangVideoMeta* pvmd,YangFrame *videoFrame){
	memset(pvmd,0,sizeof(YangVideoMeta));
	uint32_t spsLen=yang_get_be32(videoFrame->payload);
	uint32_t ppslen=yang_get_be32(videoFrame->payload+spsLen+4);
	pvmd->mp4Meta.spsLen=spsLen;
	pvmd->mp4Meta.ppsLen=ppslen;
	memcpy(pvmd->mp4Meta.sps,videoFrame->payload+4,spsLen);
	memcpy(pvmd->mp4Meta.pps,videoFrame->payload+4+spsLen+4,ppslen);

}
void yang_createH265Meta(YangVideoMeta* pvmd,YangFrame *videoFrame){
	memset(pvmd,0,sizeof(YangVideoMeta));
	uint32_t spsLen=yang_get_be32(videoFrame->payload);
	uint32_t ppslen=yang_get_be32(videoFrame->payload+spsLen+4);
	pvmd->mp4Meta.spsLen=spsLen;
	pvmd->mp4Meta.ppsLen=ppslen;
	memcpy(pvmd->mp4Meta.sps,videoFrame->payload+4,spsLen);
	memcpy(pvmd->mp4Meta.pps,videoFrame->payload+4+spsLen+4,ppslen);

}


void yang_getConfig_Flv_H264(YangH2645Conf *p_264, uint8_t *configBuf,
		int32_t *p_configLen) {

	configBuf[0] = 0x17;
	configBuf[1] = 0x00;
	configBuf[2] = 0x00;
	configBuf[3] = 0x00;
	configBuf[4] = 0x00;
	configBuf[5] = 0x01;
	configBuf[6] = p_264->sps[1];//0x42;
	configBuf[7] = p_264->sps[2];//0xC0;
	configBuf[8] = p_264->sps[3];//0x29;		//0x29;  //AVCLevelIndication1f
	configBuf[9] = 0xff;		//03;//ff;//0x03; AVCLevelIndication
	configBuf[10] = 0xe1;		//01;//e1;//01;numOfSequenceParameterSets
	uint8_t * szTmp =  configBuf + 11;


    *szTmp++=0x00;
    *szTmp++=p_264->spsLen;

	memcpy(szTmp, p_264->sps, p_264->spsLen);
	szTmp += p_264->spsLen;
	*szTmp = 0x01;
	szTmp += 1;
    *szTmp++=0x00;
    *szTmp++=p_264->ppsLen;
	memcpy(szTmp, p_264->pps, p_264->ppsLen);

	szTmp += p_264->ppsLen;
	*p_configLen = szTmp - configBuf;
	szTmp = NULL;
}
/*
 * 	uint8_t *p = videoFrame->payload;
	*p++ = 0x17;
	*p++ = 0x0; // avc_type: sequence header
	*p++ = 0x0; // composition time
	*p++ = 0x0;
	*p++ = 0x0;
	*p++ = 0x01; // version
	*p++ = sps[1];
	*p++ = sps[2];
	*p++ = sps[3];
	*p++ = 0xff;
	*p++ = 0xe1;
	yang_put_be16((char*) p, (uint16_t) spsLen);
	p += 2;
	memcpy(p, sps, spsLen);
	p += spsLen;
	yang_put_be16((char*) p, (uint16_t) ppsLen);
	p += 2;
	memcpy(p, pps, ppsLen);
	p += ppsLen;
 *
 * */
void yang_getConfig_Meta_H264(YangSample* psps, YangSample* ppps,uint8_t *configBuf,int32_t *p_configLen){
	//type_codec1 + avc_type + composition time + fix header + count of sps + len of sps + sps + count of pps + len of pps + pps
				// int32_t nb_payload = 1 + 1 + 3 + 5 + 1 + 2 + sps->size + 1 + 2 + pps->size;
	int32_t spsLen=psps->nb;
	int32_t ppsLen=psps->nb;
	uint8_t* sps=(uint8_t*)psps->bytes;
	uint8_t* pps=(uint8_t*)ppps->bytes;
	configBuf[0] = 0x17;
	configBuf[1] = 0x00;
	configBuf[2] = 0x00;
	configBuf[3] = 0x00;
	configBuf[4] = 0x00;
	configBuf[5] = 0x01;
	configBuf[6] = sps[1];//0x42;
	configBuf[7] = sps[2];//0xC0;
	configBuf[8] = sps[3];//0x29;		//0x29;  //AVCLevelIndication1f
	configBuf[9] = 0xff;		//03;//ff;//0x03; AVCLevelIndication
	configBuf[10] = 0xe1;		//01;//e1;//01;numOfSequenceParameterSets
	uint8_t * szTmp = configBuf + 11;

	yang_put_be16((char*) szTmp, (uint16_t) spsLen);
	szTmp+=2;
    //*szTmp++=0x00;
   // *szTmp++=spsLen;

	memcpy(szTmp, sps, spsLen);
	szTmp += spsLen;
	*szTmp = 0x01;
	szTmp += 1;

	yang_put_be16((char*) szTmp, (uint16_t) ppsLen);
	szTmp+=2;
   // *szTmp++=0x00;
   // *szTmp++=ppsLen;
	memcpy(szTmp, pps, ppsLen);

	szTmp += ppsLen;
	*p_configLen = szTmp -  configBuf;
	szTmp = NULL;
}
void yang_getConfig_Flv_H265(YangH2645Conf *p_264,	uint8_t *configBuf, int32_t *p_configLen) {
	int32_t i = 0;
	configBuf[i++] = 0x1C;
	configBuf[i++] = 0x00;

	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
//configurationVersion
	configBuf[i++] = 0x00;


	configBuf[i++] = p_264->sps[1];
//general_profile_compatibility_flags 32 bit
	configBuf[i++] = p_264->sps[2];
	configBuf[i++] = p_264->sps[3];
	configBuf[i++] = p_264->sps[4];
	configBuf[i++] = p_264->sps[5];

// 48 bit NUll nothing deal in rtmp
	configBuf[i++] = p_264->sps[6];
	configBuf[i++] = p_264->sps[7];
	configBuf[i++] = p_264->sps[8];
	configBuf[i++] = p_264->sps[9];
	configBuf[i++] = p_264->sps[10];
	configBuf[i++] = p_264->sps[11];

//general_level_idc
	configBuf[i++] = p_264->sps[12];

//configBuf[i++] = p_264->sps[13];
//configBuf[i++] = p_264->sps[14];

//configBuf[i++] = 0x00;
//configBuf[i++] = 0x00;
//configBuf[i++] = 0x00;
//configBuf[i++] = 0x00;
// 48 bit NUll nothing deal in rtmp
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;

//bit(16) avgFrameRate;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;

//bit(2) constantFrameRate;
// bit(3) numTemporalLayers;
///bit(1) temporalIdNested;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x03;

//yang_trace("HEVCDecoderConfigurationRecord data = %s\n", configBuf);
	configBuf[i++] = 0x20;  //vps 32
	configBuf[i++] = 0x0;
	configBuf[i++] = 0x01;
	configBuf[i++] = (p_264->vpsLen >> 8) & 0xff;
	configBuf[i++] = (p_264->vpsLen) & 0xff;
	memcpy(&configBuf[i], p_264->vps, p_264->vpsLen);
	i += p_264->vpsLen;

	configBuf[i++] = 0x21; //sps 33
	configBuf[i++] = (1 >> 8) & 0xff;
	configBuf[i++] = 1 & 0xff;
	configBuf[i++] = (p_264->spsLen >> 8) & 0xff;
	configBuf[i++] = (p_264->spsLen) & 0xff;
	memcpy(&configBuf[i], p_264->sps, p_264->spsLen);
	i += p_264->spsLen;

	configBuf[i++] = 0x22; //sps 33
	configBuf[i++] = (1 >> 8) & 0xff;
	configBuf[i++] = 1 & 0xff;
	configBuf[i++] = (p_264->ppsLen >> 8) & 0xff;
	configBuf[i++] = (p_264->ppsLen) & 0xff;
	memcpy(&configBuf[i], p_264->pps, p_264->ppsLen);
	i += p_264->ppsLen;
	*p_configLen = i;

}
void yang_getConfig_Meta_H265(YangSample* pvps,YangSample* psps, YangSample* ppps, uint8_t *configBuf,int32_t *p_configLen){
	int32_t i = 0;
	int32_t vpsLen=pvps->nb;
	int32_t spsLen=psps->nb;
	int32_t ppsLen=ppps->nb;
	uint8_t* vps=(uint8_t*)pvps->bytes;
	uint8_t* sps=(uint8_t*)psps->bytes;
	uint8_t* pps=(uint8_t*)ppps->bytes;
		configBuf[i++] = 0x1C;
		configBuf[i++] = 0x00;

		configBuf[i++] = 0x00;
		configBuf[i++] = 0x00;
		configBuf[i++] = 0x00;
	//configurationVersion
		configBuf[i++] = 0x00;


		configBuf[i++] = sps[1];
	//general_profile_compatibility_flags 32 bit
		configBuf[i++] = sps[2];
		configBuf[i++] = sps[3];
		configBuf[i++] = sps[4];
		configBuf[i++] = sps[5];

	// 48 bit NUll nothing deal in rtmp
		configBuf[i++] = sps[6];
		configBuf[i++] = sps[7];
		configBuf[i++] = sps[8];
		configBuf[i++] = sps[9];
		configBuf[i++] = sps[10];
		configBuf[i++] = sps[11];

	//general_level_idc
		configBuf[i++] = sps[12];

	//configBuf[i++] = sps[13];
	//configBuf[i++] = sps[14];

	//configBuf[i++] = 0x00;
	//configBuf[i++] = 0x00;
	//configBuf[i++] = 0x00;
	//configBuf[i++] = 0x00;
	// 48 bit NUll nothing deal in rtmp
		configBuf[i++] = 0x00;
		configBuf[i++] = 0x00;
		configBuf[i++] = 0x00;
		configBuf[i++] = 0x00;
		configBuf[i++] = 0x00;
		configBuf[i++] = 0x00;

	//bit(16) avgFrameRate;
		configBuf[i++] = 0x00;
		configBuf[i++] = 0x00;

	//bit(2) constantFrameRate;
	// bit(3) numTemporalLayers;
	///bit(1) temporalIdNested;
		configBuf[i++] = 0x00;
		configBuf[i++] = 0x03;

	//yang_trace("HEVCDecoderConfigurationRecord data = %s\n", configBuf);
		configBuf[i++] = 0x20;  //vps 32
		configBuf[i++] = 0x0;
		configBuf[i++] = 0x01;
		configBuf[i++] = (vpsLen >> 8) & 0xff;
		configBuf[i++] = (vpsLen) & 0xff;
		///yang_put_be16((char*) configBuf, (uint16_t) vpsLen);
		//i+=2;
		memcpy(&configBuf[i], vps, vpsLen);
		i += vpsLen;

		configBuf[i++] = 0x21; //sps 33
		configBuf[i++] = (1 >> 8) & 0xff;
		configBuf[i++] = 1 & 0xff;
		configBuf[i++] = (spsLen >> 8) & 0xff;
		configBuf[i++] = (spsLen) & 0xff;
		//yang_put_be16((char*) configBuf, (uint16_t) spsLen);
		//i+=2;
		memcpy(&configBuf[i], sps, spsLen);
		i += spsLen;

		configBuf[i++] = 0x22; //sps 33
		configBuf[i++] = (1 >> 8) & 0xff;
		configBuf[i++] = 1 & 0xff;
		configBuf[i++] = (ppsLen >> 8) & 0xff;
		configBuf[i++] = (ppsLen) & 0xff;
		//yang_put_be16((char*) configBuf[i], (uint16_t) ppsLen);
		//i+=2;
		memcpy(&configBuf[i], pps, ppsLen);
		i += ppsLen;
		*p_configLen = i;
}


void yang_getH265RtmpHeader(uint8_t *meta, uint8_t *src, int32_t *hLen) {
				uint8_t s[4];
				s[0]=0x00;
				s[1]=0x00;
				s[2]=0x00;
				s[3]=0x01;

				int32_t pos=0;
				int32_t vpsLen=*(meta+32);
				int32_t vpsPos=33;

				int32_t spsLen=*(meta+32+vpsLen+5);
				int32_t spsPos=32+vpsLen+5+1;

				int32_t ppsLen=*(meta+32+vpsLen+5+spsLen+5);
				int32_t ppsPos=32+vpsLen+5+spsLen+5+1;

				//printf("\n***************vpsLen=%d,spsLen=%d,ppsLen=%d\n",vpsLen,spsLen,ppsLen);
				memcpy(src,s,4);
				memcpy(src+4,meta+vpsPos,vpsLen);
				pos+=4+vpsLen;
				memcpy(src+pos,s,4);
				pos+=4;
				memcpy(src+pos,meta+spsPos,spsLen);
				pos+=spsLen;
				memcpy(src+pos,s,4);
				pos+=4;
				memcpy(src+pos,meta+ppsPos,ppsLen);
				pos+=ppsLen;
				*hLen=pos;

}

void yang_getH264RtmpHeader(uint8_t *buf, uint8_t *src, int32_t *hLen) {
	int32_t spsLen = *(buf + 12) + 1;
	uint8_t *spsStart = buf + 13;
	int32_t ppsLen = *(spsStart + spsLen + 1) + 1;
	uint8_t *ppsStart = buf + 13 + spsLen + 2;
	*(src + 3) = 0x01;
	memcpy(src + 4, spsStart, spsLen);
	*(src + 4 + spsLen + 3) = 0x01;
	memcpy(src + 4 + spsLen + 4, ppsStart, ppsLen);
	*hLen = 8 + spsLen + ppsLen;
}

void yang_decodeMetaH264(uint8_t *buf,int32_t p_configLen,YangSample* sps,YangSample* pps){
	sps->nb= *(buf + 12) + 1;
	sps->bytes = (char*)buf + 13;
	pps->nb = *(sps->bytes + sps->nb + 1) + 1;
	pps->bytes= (char*)buf + 13 + sps->nb + 2;
}
void yang_decodeMetaH265(uint8_t *meta,int32_t p_configLen,YangSample* vps,YangSample* sps,YangSample* pps){
	vps->nb=*(meta+32);
	vps->bytes=(char*)meta+33;
	int32_t vpsLen=vps->nb;
	sps->nb=*(meta+32+vpsLen+5);
	int32_t spsLen=sps->nb;
	sps->bytes=(char*)meta+32+vpsLen+5+1;

	pps->nb=*(meta+32+vpsLen+5+spsLen+5);
	pps->bytes=(char*)meta+32+vpsLen+5+spsLen+5+1;
}

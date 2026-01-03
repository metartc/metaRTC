//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangvideo/YangCMeta.h>
#include <yangvideo/YangCNalu.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>

#include <yangutil/yangavinfo.h>


void yang_find_start_code(YangVideoCodec pve,uint8_t *buf,int32_t bufLen,int32_t *vpsPos,int32_t *vpsLen,int32_t *spsPos,int32_t *spsLen,int32_t *ppsPos,int32_t *ppsLen)
{
   int32_t i = 0;
   *spsPos=0;*ppsPos=0;
   if(pve==Yang_VED_H265)	  {
	   *vpsPos=0;
	   while(i<bufLen-3){
		      if (buf[i] == 0 && buf[i + 1] == 0 &&buf[i + 2] == 0&& buf[i + 3] == 1){
		       	*vpsPos=i+4;
		       	i+=4;
		       	 break;
		       }
		      i++;
	   }
   }
   while (i <bufLen-3) {
       if (buf[i] == 0 && buf[i + 1] == 0 &&buf[i + 2] == 0&& buf[i + 3] == 1){
    	   if(pve==Yang_VED_H265) *vpsLen=i-4;
       	*spsPos=i+4;
       	i+=4;
       	 break;
       }

       i++;
   }
   while (i <bufLen-3) {
       if (buf[i] == 0 && buf[i + 1] == 0 &&buf[i + 2] == 0&& buf[i + 3] == 1){
       	*spsLen=i-*spsPos;
       	*ppsPos=i+4;
       	*ppsLen=bufLen-*ppsPos;
       	 break;
       }

       i++;
   }

}
int32_t yang_find_pre_start_code(uint8_t *buf,int32_t bufLen){
	int32_t i = 0;
	while(i<bufLen-3){
		if (buf[i] == 0 && buf[i + 1] == 0 &&buf[i + 2] == 0&& buf[i + 3] == 1){
			return i;
			break;
		}
		i++;
	}
	return -1;
}

int32_t yang_meta_parseH264(YangH2645Meta* meta,uint8_t *payload,int32_t  nb){
	int32_t bufLen = yang_max(256,nb);
	int32_t spsPos=0,ppsPos=0,seiPos=0;

	yang_memset(meta,0,sizeof(YangH2645Meta));

	//ipos=yang_nalu_getH264KeyframePos(payload,bufLen);
	//if(ipos<0) return 1;

	spsPos=yang_find_pre_start_code(payload,bufLen);

	if(spsPos<0)
		return 1;

	ppsPos=yang_find_pre_start_code(payload+4+spsPos,bufLen-4-spsPos);

	if(ppsPos<0)
		return 1;

	ppsPos+=4+spsPos;

	seiPos=yang_find_pre_start_code(payload+4+ppsPos,bufLen-4-ppsPos);

	if(seiPos<0)
		return 1;

	seiPos+=4+ppsPos;

	meta->spsPos=spsPos+4;
	meta->ppsPos=ppsPos+4;

	meta->spsLen=ppsPos-spsPos-4;
	meta->ppsLen=seiPos-ppsPos-4;
	return Yang_Ok;
}

int32_t yang_meta_parseH265(YangH2645Meta* meta,uint8_t *payload,int32_t  nb){
	int32_t bufLen = yang_max(256,nb);

	int32_t vpsPos=0,spsPos=0,ppsPos=0,seiPos=0;

	yang_memset(meta,0,sizeof(YangH2645Meta));

	vpsPos=yang_find_pre_start_code(payload,bufLen);

	if(vpsPos<0)
		return 1;

	spsPos=yang_find_pre_start_code(payload+4+vpsPos,bufLen-4-vpsPos);

	if(spsPos<0)
		return 1;

	spsPos+=4+vpsPos;

	ppsPos=yang_find_pre_start_code(payload+4+spsPos,bufLen-4-spsPos);

	if(ppsPos<0)
		return 1;

	ppsPos+=4+spsPos;

	seiPos=yang_find_pre_start_code(payload+4+ppsPos,bufLen-4-ppsPos);

	if(seiPos<0)
		return 1;

	seiPos+=4+ppsPos;
	meta->vpsPos=vpsPos+4;
	meta->spsPos=spsPos+4;
	meta->ppsPos=ppsPos+4;
	meta->vpsLen=spsPos-vpsPos-4;
	meta->spsLen=ppsPos-spsPos-4;
	meta->ppsLen=seiPos-ppsPos-4;

	return Yang_Ok;
}


int32_t yang_meta_createH264( YangH2645Conf* conf, uint8_t* payload,int32_t nb){
	YangH2645Meta meta={0};

	if(yang_meta_parseH264(&meta,payload,nb))
		return 1;

	conf->spsLen=meta.spsLen;
	conf->ppsLen=meta.ppsLen;
	yang_memcpy(conf->sps,payload+meta.spsPos,meta.spsLen);
	yang_memcpy(conf->pps,payload+meta.ppsPos,meta.ppsLen);
	return Yang_Ok;

}


int32_t yang_meta_createH265( YangH2645Conf* conf, uint8_t* payload,int32_t nb){

	YangH2645Meta meta={0};

	if(yang_meta_parseH265(&meta,payload,nb))
		return 1;

	conf->vpsLen=meta.vpsLen;
	conf->spsLen=meta.spsLen;
	conf->ppsLen=meta.ppsLen;
	yang_memcpy(conf->vps,payload+meta.vpsPos,meta.vpsLen);
	yang_memcpy(conf->sps,payload+meta.spsPos,meta.spsLen);
	yang_memcpy(conf->pps,payload+meta.ppsPos,meta.ppsLen);
	return Yang_Ok;
}

int32_t yang_meta_getH264Sps(uint8_t* payload,int32_t nb,uint8_t** sps,int32_t* spsLen){
	int32_t keyframePos=yang_nalu_getH264KeyframePos(payload,nb);
	YangH2645Meta meta={0};

	if(keyframePos==-1)
		return 1;

	yang_meta_parseH264(&meta,payload,keyframePos);


	*spsLen = meta.spsLen;
	*sps = payload+meta.spsPos;
	return Yang_Ok;
}

int32_t yang_meta_getH265Sps(uint8_t *payload,int32_t nb, uint8_t **sps, int32_t *spsLen) {

	int32_t keyframePos=yang_nalu_getH265KeyframePos(payload,nb);
	YangH2645Meta meta={0};

	if(keyframePos==-1)
		return 1;

	yang_meta_parseH265(&meta,payload,keyframePos);


	*spsLen = meta.spsLen;
	*sps = payload+meta.spsPos;
	return Yang_Ok;
}


void yang_meta_getH264Flv( YangH2645Conf *p_264, uint8_t *configBuf,
		int32_t *p_configLen) {
	uint8_t * szTmp;
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
	szTmp =  configBuf + 11;


    *szTmp++=0x00;
    *szTmp++=p_264->spsLen;

	yang_memcpy(szTmp, p_264->sps, p_264->spsLen);
	szTmp += p_264->spsLen;
	*szTmp = 0x01;
	szTmp += 1;
    *szTmp++=0x00;
    *szTmp++=p_264->ppsLen;
	yang_memcpy(szTmp, p_264->pps, p_264->ppsLen);

	szTmp += p_264->ppsLen;
	*p_configLen = szTmp - configBuf;
	szTmp = NULL;
}

void yang_meta_getH264FlvBySample( YangSample* psps,  YangSample* ppps,uint8_t *configBuf,int32_t *p_configLen){
	//type_codec1 + avc_type + composition time + fix header + count of sps + len of sps + sps + count of pps + len of pps + pps
				// int32_t nb_payload = 1 + 1 + 3 + 5 + 1 + 2 + sps->size + 1 + 2 + pps->size;
	uint8_t * szTmp;
	int32_t spsLen=psps->nb;
	int32_t ppsLen=ppps->nb;
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
	szTmp = configBuf + 11;

	yang_put_be16((char*) szTmp, (uint16_t) spsLen);
	szTmp+=2;
    //*szTmp++=0x00;
   // *szTmp++=spsLen;

	yang_memcpy(szTmp, sps, spsLen);
	szTmp += spsLen;
	*szTmp = 0x01;
	szTmp += 1;

	yang_put_be16((char*) szTmp, (uint16_t) ppsLen);
	szTmp+=2;
   // *szTmp++=0x00;
   // *szTmp++=ppsLen;
	yang_memcpy(szTmp, pps, ppsLen);

	szTmp += ppsLen;
	*p_configLen = szTmp -  configBuf;
	szTmp = NULL;
}

void yang_meta_getH265Flv( YangH2645Conf *p_264,	uint8_t *configBuf, int32_t *p_configLen) {
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
	yang_memcpy(&configBuf[i], p_264->vps, p_264->vpsLen);
	i += p_264->vpsLen;

	configBuf[i++] = 0x21; //sps 33
	configBuf[i++] = (1 >> 8) & 0xff;
	configBuf[i++] = 1 & 0xff;
	configBuf[i++] = (p_264->spsLen >> 8) & 0xff;
	configBuf[i++] = (p_264->spsLen) & 0xff;
	yang_memcpy(&configBuf[i], p_264->sps, p_264->spsLen);
	i += p_264->spsLen;

	configBuf[i++] = 0x22; //sps 33
	configBuf[i++] = (1 >> 8) & 0xff;
	configBuf[i++] = 1 & 0xff;
	configBuf[i++] = (p_264->ppsLen >> 8) & 0xff;
	configBuf[i++] = (p_264->ppsLen) & 0xff;
	yang_memcpy(&configBuf[i], p_264->pps, p_264->ppsLen);
	i += p_264->ppsLen;
	*p_configLen = i;

}
void yang_meta_getH265FlvBySample( YangSample* pvps, YangSample* psps,  YangSample* ppps, uint8_t *configBuf,int32_t *p_configLen){
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


	configBuf[i++] = 0x20;  //vps 32
	configBuf[i++] = 0x0;
	configBuf[i++] = 0x01;
	configBuf[i++] = (vpsLen >> 8) & 0xff;
	configBuf[i++] = (vpsLen) & 0xff;

	yang_memcpy(&configBuf[i], vps, vpsLen);
	i += vpsLen;

	configBuf[i++] = 0x21; //sps 33
	configBuf[i++] = (1 >> 8) & 0xff;
	configBuf[i++] = 1 & 0xff;
	configBuf[i++] = (spsLen >> 8) & 0xff;
	configBuf[i++] = (spsLen) & 0xff;

	yang_memcpy(&configBuf[i], sps, spsLen);
	i += spsLen;

	configBuf[i++] = 0x22; //sps 33
	configBuf[i++] = (1 >> 8) & 0xff;
	configBuf[i++] = 1 & 0xff;
	configBuf[i++] = (ppsLen >> 8) & 0xff;
	configBuf[i++] = (ppsLen) & 0xff;

	yang_memcpy(&configBuf[i], pps, ppsLen);
	i += ppsLen;
	*p_configLen = i;
}


int32_t yang_meta_parseH265Header(uint8_t *payload,int32_t nb, uint8_t *src, int32_t *hLen) {
	int32_t keyframePos=yang_nalu_getH265KeyframePos(payload,nb);

	if(keyframePos==-1)
		return 1;

	yang_memcpy(src,payload,keyframePos);
	*hLen=keyframePos;
	return Yang_Ok;

}

int32_t yang_meta_parseH264Header(uint8_t *payload,int32_t nb, uint8_t *src, int32_t *hLen) {
	int32_t keyframePos=yang_nalu_getH264KeyframePos(payload,nb);

	if(keyframePos==-1)
		return 1;

	yang_memcpy(src,payload,keyframePos);
	*hLen=keyframePos;
	return Yang_Ok;
}


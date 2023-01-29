//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangavutil/video/YangCNalu.h>
#include <yangavutil/video/YangMeta.h>

#include <yangutil/yangavh265.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>

int32_t yang_getH264KeyframeNalu( YangFrame *videoFrame) {
	uint8_t *tmp = NULL;
	int len = videoFrame->nb;
	uint32_t naluLen = 0;
	int pos = 0;
	int32_t err = 1;
	while (pos < len) {
		tmp = videoFrame->payload + pos;
		if ((*(tmp + 4) & kNalTypeMask) == YangAvcNaluTypeIDR) {
			videoFrame->payload = tmp;
			videoFrame->nb = len - pos;
			err = Yang_Ok;
			break;
		}
		naluLen = yang_get_be32(tmp);
		if (naluLen > len) {
			break;
		}
		pos += naluLen + 4;
	}
	return err;
}

yangbool yang_hasH264Pframe(uint8_t *p) {
	if ((*(p + 4) & kNalTypeMask) == YangAvcNaluTypeNonIDR)
		return yangtrue;
	return yangfalse;
}



int32_t yang_parseH264Nalu( YangFrame *videoFrame,  YangH264NaluData *pnalu) {
	uint8_t *tmp = NULL;
	uint32_t len = videoFrame->nb;
	uint32_t naluLen = 0;
	int32_t pos = 0;
	int32_t err = Yang_Ok;
	pnalu->spsppsPos = -1;
	pnalu->keyframePos = -1;
	while (pos < len) {
		tmp = videoFrame->payload + pos;
		if ((*(tmp + 4) & kNalTypeMask) == YangAvcNaluTypeIDR) {
			pnalu->keyframePos = pos;
			break;
		}
		if ((*(tmp + 4) & kNalTypeMask) == YangAvcNaluTypeSPS) {
			pnalu->spsppsPos = pos;
		}
		naluLen = yang_get_be32(tmp);
		if (naluLen > len) {
			break;
		}
		pos += naluLen + 4;
	}
	return err;
}


int32_t yang_getNalupos(uint8_t* data,int plen){
	uint8_t* p=NULL;
	int len=plen-4;
	for(int i=0;i<len;i++){
		p=data+i;
		if((*p)==0x00&&(*(p+1))==0x00&&(*(p+2))==0x00&&(*(p+3))==0x01){
			return i+4;
		}

	}
	return -1;
}

int32_t yang_getNalupos2(uint8_t* data, int plen, int* prefix)
{
	uint8_t* p = NULL;
	int len = plen - 4;
	for (int i = 0; i < len; i++) {
		p = data + i;
		if ((*p) == 0x00 && (*(p + 1)) == 0x00 && (*(p + 2)) == 0x00 && (*(p + 3)) == 0x01) {
			*prefix = 4;
			return i + 4;
		}
		if ((*p) == 0x00 && (*(p + 1)) == 0x00 && (*(p + 2)) == 0x01)
		{
			*prefix = 3;
			return i + 3;
		}
	}
	return -1;
}

int32_t yang_parseH264Nalu2( YangFrame *videoFrame,  YangH264NaluData2 *pnalu) {
	uint8_t *tmp = NULL;
	uint32_t len = videoFrame->nb;
	//uint32_t naluLen = 0;
	int32_t pos = 0;
	int32_t err = 1;
	int32_t tmpPos=0;
	int32_t tmpLen=len;
	pnalu->spsPos = -1;
	pnalu->ppsPos = -1;
	pnalu->keyframePos = -1;
	pnalu->spsLen=0;
	pnalu->ppsLen=0;
	int32_t preType=-1;
	int32_t prefix=4;
	while (pos < len) {

		tmp = videoFrame->payload + pos;
		//tmpPos=yang_getNalupos(tmp,tmpLen);
		tmpPos=yang_getNalupos2(tmp,tmpLen,&prefix);
		if(tmpPos==-1) return 1;

		if(preType==0){
			pnalu->spsLen=tmpPos-prefix;
			preType=-1;
		}
		if(preType==1){
				pnalu->ppsLen=tmpPos-prefix;
				preType=-1;
			}
		tmp+=tmpPos;

		if ((*tmp & kNalTypeMask) == YangAvcNaluTypeIDR) {
			pnalu->keyframePos =tmp-videoFrame->payload;
			preType=2;
			return Yang_Ok;
		}
		if ((*tmp & kNalTypeMask) == YangAvcNaluTypeSPS) {
			pnalu->spsPos = tmp-videoFrame->payload;
			preType=0;
		}
		if ((*tmp & kNalTypeMask) == YangAvcNaluTypePPS) {
					pnalu->ppsPos = tmp-videoFrame->payload;
					preType=1;
		}
		tmpLen-=tmpPos;
		pos+=tmpPos;



	}
	return err;
}
int32_t yang_getH264SpsppseNalu( YangFrame *videoFrame, uint8_t *pnaludata) {
	if (!videoFrame || !pnaludata || !videoFrame->payload)
		return yang_error_wrap(1, "getSpsppseNalu is null");
	int32_t spsLen = yang_get_be32(pnaludata);
	int32_t ppsLen = yang_get_be32(pnaludata + 4 + spsLen);
	if(spsLen+ppsLen+8>Yang_Nalu_SpsppsBufferLen) return 1;
	if (spsLen > videoFrame->nb || ppsLen > videoFrame->nb)		return 1;
	uint8_t *sps = pnaludata + 4;
	uint8_t *pps = pnaludata + 4 + spsLen + 4;

	 YangSample sps_sample,pps_sample;
    sps_sample.bytes=(char*)sps;
	sps_sample.nb=spsLen;

    pps_sample.bytes=(char*)pps;
	pps_sample.nb=ppsLen;


	yang_getConfig_Meta_H264(&sps_sample,&pps_sample,videoFrame->payload,&videoFrame->nb);

	return Yang_Ok;
}




int32_t yang_parseH265Nalu( YangFrame *videoFrame,  YangH264NaluData *pnalu) {
	uint8_t *tmp = NULL; //videoFrame->payload;
	uint32_t len = videoFrame->nb;
	uint32_t naluLen = 0;
	int32_t pos = 0;
	int32_t err = Yang_Ok;
	pnalu->spsppsPos = -1;
	pnalu->keyframePos = -1;
	int32_t v=0;
	while (pos < len) {
		tmp = videoFrame->payload + pos;
		v=YANG_HEVC_NALU_TYPE(*(tmp + 4));

		if (v >= YANG_NAL_UNIT_CODED_SLICE_BLA&&v<= YANG_NAL_UNIT_CODED_SLICE_CRA) {
			pnalu->keyframePos = pos;
			break;
		}
		if (v== YANG_NAL_UNIT_VPS) {
			pnalu->spsppsPos = pos;
		}
		naluLen = yang_get_be32(tmp);
		if (naluLen > len) {
			break;
		}
		pos += naluLen + 4;
	}
	return err;
}
int32_t yang_getH265SpsppseNalu( YangFrame *videoFrame, uint8_t *pnaludata) {
	if (!videoFrame || !pnaludata || !videoFrame->payload)
		return yang_error_wrap(1, "getSpsppseNalu is null");
	 YangSample vps_sample,sps_sample,pps_sample;
	int32_t vpsLen = yang_get_be32(pnaludata);
	int32_t spsLen = yang_get_be32(pnaludata+ 4 + vpsLen);
	int32_t ppsLen = yang_get_be32(pnaludata + 4 + vpsLen+4+spsLen);
	if(vpsLen+spsLen+ppsLen+8>Yang_Nalu_SpsppsBufferLen) return 1;
	if (spsLen > videoFrame->nb || ppsLen > videoFrame->nb)
		return 1;
	uint8_t *vps = pnaludata + 4;
	uint8_t *sps = pnaludata + 4 + vpsLen + 4;
	uint8_t *pps=pnaludata + 4 + vpsLen + 4+spsLen+4;

    vps_sample.bytes=(char*)vps;
	vps_sample.nb=vpsLen;

    sps_sample.bytes=(char*)sps;
	sps_sample.nb=spsLen;

    pps_sample.bytes=(char*)pps;
	pps_sample.nb=ppsLen;

	yang_getConfig_Meta_H265(&vps_sample,&sps_sample,&pps_sample,videoFrame->payload,&videoFrame->nb);

	return Yang_Ok;
}

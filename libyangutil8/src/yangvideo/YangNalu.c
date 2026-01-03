//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangvideo/YangCNalu.h>
#include <yangvideo/YangMeta.h>

#include <yangutil/yangavinfo.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
int32_t yang_nalu_getH264KeyframePos( uint8_t* data,int32_t nb) {
	uint8_t *tmp = data;
	int32_t len = 0;
	int32_t pos = 0;
	int32_t left=nb;

	while (len < nb) {

		len=tmp-data;
		if ((*(tmp + 4) & kNalTypeMask) == YangAvcNaluTypeIDR) {
			return len;
		}

		left=nb-4-pos;
		tmp=tmp+4;
		pos=yang_find_pre_start_code(tmp,left);

		if(pos==-1)
			break;

		tmp = tmp + pos;
	}
	return -1;
}

int32_t yang_nalu_getH265KeyframePos( uint8_t* data,int32_t nb) {
	uint8_t *tmp = data;
	uint8_t nalutype=0;
	int32_t len = 0;
	int32_t pos = 0;
	int32_t left=nb;

	while (len < nb) {

		len=tmp-data;
		nalutype=YANG_HEVC_NALU_TYPE(*(tmp + 4));

		if (nalutype >= YANG_NAL_UNIT_CODED_SLICE_BLA&&nalutype <= YANG_NAL_UNIT_CODED_SLICE_CRA) {
			return len;
		}

		left=nb-4-pos;
		tmp=tmp+4;
		pos=yang_find_pre_start_code(tmp,left);

		if(pos==-1)
			break;

		tmp = tmp + pos;
	}
	return -1;
}

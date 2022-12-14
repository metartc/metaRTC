//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsdp/YangRtxPayloadDes.h>


void yang_create_rtxPayloaddes(YangRtxPayloadDes* des,uint8_t pt, char* encode_name, int32_t sample){
	des->sample=sample;
	des->pt=pt;
	yang_strcpy(des->name,(char*)"rtx");
}

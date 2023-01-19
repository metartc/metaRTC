//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsdp/YangAudioPayload.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>

void yang_init_audioPayload(YangAudioPayload* audio,int32_t channel){
	    yang_strcpy(audio->codecPayload.type ,"audio");
	    audio->channel = channel;
	    audio->opus_param.minptime = 0;
	    audio->opus_param.use_inband_fec = 0;
	    audio->opus_param.usedtx = 0;
}
int32_t yang_sdp_set_opus_param_desc(YangAudioPayload* audio,char* fmtp)
{
    int32_t err = Yang_Ok;
    YangStrings vec;
    yang_cstr_split(fmtp,";",&vec);

    for (size_t i = 0; i < vec.vsize; ++i) {
    	YangStrings kv;
    	 yang_cstr_split(vec.str[i],"=",&kv);

        if (kv.vsize == 2) {
            if (yang_strcmp(kv.str[0] ,"minptime")==0) {
                audio->opus_param.minptime = (int32_t)atol(kv.str[1]);
            } else if (yang_strcmp(kv.str[0] , "useinbandfec")==0) {
            	audio->opus_param.use_inband_fec = (((int32_t)atol(kv.str[1])) == 1) ? 1 : 0;
            } else if (yang_strcmp(kv.str[0] , "usedtx")==0) {
            	audio->opus_param.usedtx = (((int32_t)atol(kv.str[1])) == 1) ? 1 : 0;
            }
            yang_destroy_strings(&kv);
        } else {
        	yang_destroy_strings(&kv);
        	yang_destroy_strings(&vec);
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid opus param=%s", vec.str[i]);
        }
    }
    yang_destroy_strings(&vec);
    return err;
}

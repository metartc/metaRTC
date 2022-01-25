
#include <yangsdp/YangSSRCInfo.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangCString.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define kCRLF  (char*)"\r\n"
yang_impl_struct_init(YangSSRCInfo)
yang_impl_struct_insert(YangSSRCInfo)
yang_impl_struct_destroy(YangSSRCInfo)

yang_impl_struct_insert(YangSSRCGroup)
yang_impl_struct_init(YangSSRCGroup)
yang_impl_struct_destroy(YangSSRCGroup)


yang_impl_struct_insert(yangsdpint)
yang_impl_struct_init(yangsdpint)
yang_impl_struct_destroy(yangsdpint)

int32_t yang_encode_ssrcinfo(YangSSRCInfo* info,YangBuffer* os)
{
    int32_t err = Yang_Ok;

    if (info->ssrc == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrc");
    }
    char ssrcstr[32];
    memset(ssrcstr,0,sizeof(ssrcstr));
    yang_itoa((int)info->ssrc,ssrcstr,10);
    yang_write_cstring(os,"a=ssrc:");
    yang_write_cstring(os,ssrcstr);
    yang_write_cstring(os," cname:");
    yang_write_cstring(os,info->cname);
    yang_write_cstring(os,kCRLF);

    if (strlen(info->msid)) {
    	 yang_write_cstring(os,"a=ssrc:");
    	 yang_write_cstring(os,ssrcstr);
    	 yang_write_cstring(os, " msid:");
    	 yang_write_cstring(os, info->msid);

        if (strlen(info->msid_tracker)) {
       	 yang_write_cstring(os, " ");
           	 yang_write_cstring(os, info->msid_tracker);

        }
        yang_write_cstring(os,kCRLF);

    }
    if (strlen(info->mslabel)) {
        yang_write_cstring(os,"a=ssrc:");
        yang_write_cstring(os,ssrcstr);
        yang_write_cstring(os," mslabel:");
        yang_write_cstring(os,info->mslabel);
        yang_write_cstring(os,kCRLF);

    }
    if (strlen(info->label)) {
        yang_write_cstring(os,"a=ssrc:");
          yang_write_cstring(os,ssrcstr);
          yang_write_cstring(os," label:");
          yang_write_cstring(os,info->label);
          yang_write_cstring(os,kCRLF);

    }

    return err;
}

int32_t yang_encode_ssrcgroup(YangSSRCGroup* group,YangBuffer* os)
{
    int32_t err = Yang_Ok;

    if (strlen(group->semantic)==0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid semantics");
    }

    if (group->groups.vlen == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrcs");
    }
    yang_write_cstring(os,"a=ssrc-group:");
    yang_write_cstring(os,group->semantic);

    char ssrcstr[32];

    for (int32_t i = 0; i < group->groups.vlen; i++) {
    	  yang_write_cstring(os," ");
    	     memset(ssrcstr,0,sizeof(ssrcstr));
    	     yang_itoa(group->groups.payload[i],ssrcstr,10);
    	  yang_write_cstring(os,ssrcstr);
      //  os << " " << m_ssrcs[i];
    }

    return err;
}

void yang_insert_strVector(YangStrVector* vec,char* str){
	if(vec->vlen>=vec->capacity){
		char** tmp=(char**)malloc(vec->vlen*sizeof(char*));
		memcpy(tmp,vec->payload,vec->vlen*sizeof(char*));
		yang_free(vec->payload);
		vec->payload=(char**)malloc((vec->capacity+10)*sizeof(char*));
		memcpy(vec->payload,tmp,vec->vlen*sizeof(char*));
		yang_free(tmp);
		vec->capacity+=10;
	}
	vec->payload[vec->vlen]=(char*)calloc(strlen(str)+1,1);
	strcpy(vec->payload[vec->vlen],str);
	vec->vlen++;
}
void yang_init_strVector(YangStrVector* vec){
	vec->capacity=10;
	vec->payload=(char**)calloc(vec->capacity*sizeof(char*),1);
	vec->vlen=0;
}

void yang_destroy_strVector(YangStrVector* vec){
	for(int i=0;i<vec->vlen;i++){
		yang_free(vec->payload[i]);
	}
	vec->vlen=0;
}
void yang_clear_strVector(YangStrVector* vec){
	yang_destroy_strVector(vec);
	memset(vec->payload,0,sizeof(char*)*vec->capacity);
}

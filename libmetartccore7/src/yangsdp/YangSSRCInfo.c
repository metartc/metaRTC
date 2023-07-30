//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsdp/YangSSRCInfo.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/buffer/YangBuffer.h>

#define kCRLF  (char*)"\r\n"
yang_vector_impl(YangSSRCInfo)
yang_vector_impl(YangSSRCGroup)
yang_vector_impl(yangsdpint)


int32_t yang_encode_ssrcinfo(YangSSRCInfo* info,YangBuffer* os)
{
    int32_t err = Yang_Ok;

    if (info->ssrc == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrc");
    }
    char ssrcstr[32];
    yang_memset(ssrcstr,0,sizeof(ssrcstr));
    yang_itoa2(info->ssrc,ssrcstr,10);
    yang_write_cstring(os,"a=ssrc:");
    yang_write_cstring(os,ssrcstr);
    yang_write_cstring(os," cname:");
    yang_write_cstring(os,info->cname);
    yang_write_cstring(os,kCRLF);

    if (yang_strlen(info->msid)) {
    	 yang_write_cstring(os,"a=ssrc:");
    	 yang_write_cstring(os,ssrcstr);
    	 yang_write_cstring(os, " msid:");
    	 yang_write_cstring(os, info->msid);

        if (yang_strlen(info->msid_tracker)) {
       	 yang_write_cstring(os, " ");
           	 yang_write_cstring(os, info->msid_tracker);

        }
        yang_write_cstring(os,kCRLF);

    }
    if (yang_strlen(info->mslabel)) {
        yang_write_cstring(os,"a=ssrc:");
        yang_write_cstring(os,ssrcstr);
        yang_write_cstring(os," mslabel:");
        yang_write_cstring(os,info->mslabel);
        yang_write_cstring(os,kCRLF);

    }
    if (yang_strlen(info->label)) {
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

    if (yang_strlen(group->semantic)==0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid semantics");
    }

    if (group->groups.vsize == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrcs");
    }
    yang_write_cstring(os,"a=ssrc-group:");
    yang_write_cstring(os,group->semantic);

    char ssrcstr[32];

    for (int32_t i = 0; i < group->groups.vsize; i++) {
    	  yang_write_cstring(os," ");
    	     yang_memset(ssrcstr,0,sizeof(ssrcstr));
    	     yang_itoa(group->groups.payload[i],ssrcstr,10);
    	  yang_write_cstring(os,ssrcstr);
      //  os << " " << m_ssrcs[i];
    }

    return err;
}


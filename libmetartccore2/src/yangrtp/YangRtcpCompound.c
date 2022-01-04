#include <yangrtp/YangCRtcpCompound.h>

#include <yangrtp/YangRtcpSR.h>
#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtcpNack.h>
#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtp/YangRtcpPli.h>
#include <yangrtp/YangRtcpSli.h>
#include <yangrtp/YangRtcpRpsi.h>

#include <yangutil/sys/YangLog.h>
#include <string.h>
void yang_init_rtcpCompound(YangRtcpCompound* rtcp){
	if(rtcp==NULL) return;
	if(rtcp->rtcps==NULL) rtcp->rtcps=(char*)calloc(1,1500);
}
void yang_destroy_rtcpCompound(YangRtcpCompound* rtcp){
	if(rtcp==NULL) return;
	if(rtcp->vlen){
		for(int i=0;i<rtcp->vlen;i++){
			YangRtcpCommon* comm=(YangRtcpCommon*)(rtcp->rtcps+i*sizeof(YangRtcpCommon));
			yang_destroy_rtcpCommon(comm);
		}
	}
	yang_free(rtcp->rtcps);
}
int32_t yang_decode_rtcpCompound(YangRtcpCompound* rtcps,YangBuffer *buffer){
	int32_t err = Yang_Ok;
	rtcps->data = buffer->data;
	rtcps->nb_data = buffer->size;

	    while (!yang_buffer_empty(buffer)) {
	        YangRtcpCommon* rtcp = (YangRtcpCommon*)calloc(1,sizeof(YangRtcpCommon));
	        YangRtcpHeader* header = (YangRtcpHeader*)(buffer->head);
	        if (header->type == YangRtcpType_sr) {
	        	yang_init_rtcpSR(rtcp);
	        	err=yang_decode_rtcpSR(rtcp,buffer);

	            //rtcp = new YangRtcpSR();
	        } else if (header->type == YangRtcpType_rr) {
	        	yang_init_rtcpRR(rtcp);
	        	err=yang_decode_rtcpRR(rtcp,buffer);
	           // rtcp = new YangRtcpRR();
	        } else if (header->type == YangRtcpType_rtpfb) {
	            if(1 == header->rc) {
	            	yang_init_rtcpNack(rtcp,0);
	                //nack
	            	yang_decode_rtcpNack(rtcp,buffer);
	                //rtcp = new YangRtcpNack();
	            } else if (15 == header->rc) {
	                //twcc
	                //rtcp = new YangRtcpTWCC();
	            }
	        } else if(header->type == YangRtcpType_psfb) {
	            if(1 == header->rc) {
	                // pli
	                //rtcp = new YangRtcpPli();
	            	yang_init_rtcpPli(rtcp);
	            	err=yang_decode_rtcpPli(rtcp,buffer);
	            } else if(2 == header->rc) {
	                //sli
	                //rtcp = new YangRtcpSli();
	            } else if(3 == header->rc) {
	                //rpsi
	               // rtcp = new YangRtcpRpsi();
	            	yang_init_rtcpRpsi(rtcp);
	            	yang_decode_rtcpRpsi(rtcp,buffer);
	            } else {
	                // common psfb
	               // rtcp = new YangRtcpPsfb();
	            	yang_init_rtcpPsfb(rtcp);
	            	err=yang_decode_rtcpPsfb(rtcp,buffer);
	            }
	        } else if(header->type == YangRtcpType_xr) {
	           // rtcp = new YangRtcpXr();
	        	yang_init_rtcpXr(rtcp);
	        	err=yang_decode_rtcpXr(rtcp,buffer);
	        } else {
	            //rtcp = new YangRtcpCommon();

	        }


	        if(err) {
	        	uint32_t header_type=rtcp->header.type;
	        	uint32_t header_rc=rtcp->header.rc;
	        	yang_free(rtcp);
	        	if(err==ERROR_RTC_RTCP_EMPTY_RR)  continue;
	        	return yang_error_wrap(err, "decode rtcp type=%u rc=%u", header_type, header_rc);
	   	    }
	        memcpy(rtcps->rtcps,rtcp,sizeof(YangRtcpCommon));
	        rtcps->vlen++;
	        yang_free(rtcp);
	       // m_rtcps.push_back(rtcp);
	    }

	    return err;
}
int32_t yang_encode_rtcpCompound(YangRtcpCompound *rtcps, YangBuffer *buffer) {
	int32_t err = Yang_Ok;
	if (!yang_buffer_require(buffer, rtcps->nb_bytes)) {
		return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes",
				rtcps->nb_bytes);
	}
	for (int i = 0; i < rtcps->vlen; i++) {
		YangRtcpCommon *rtcp = (YangRtcpCommon*) (rtcps->rtcps + i * sizeof(YangRtcpCommon));
		if (rtcp->header.type == YangRtcpType_sr) {
			err = yang_encode_rtcpSR(rtcp, buffer);
		} else if (rtcp->header.type == YangRtcpType_rr) {
			err = yang_encode_rtcpRR(rtcp, buffer);
		} else if (rtcp->header.type == YangRtcpType_rtpfb) {
			if (1 == rtcp->header.rc) {
				//nack
				//rtcp = new YangRtcpNack();
				err=yang_encode_rtcpNack(rtcp,buffer);
			} else if (15 == rtcp->header.rc) {
				//twcc
				//rtcp = new YangRtcpTWCC();
			}
		} else if (rtcp->header.type == YangRtcpType_psfb) {
			if (1 == rtcp->header.rc) {
				// pli
				err = yang_encode_rtcpPli(rtcp, buffer);
			} else if (2 == rtcp->header.rc) {
				//sli
				//rtcp = new YangRtcpSli();
			} else if (3 == rtcp->header.rc) {
				//rpsi
				//rtcp = new YangRtcpRpsi();
				err=yang_encode_rtcpRpsi(rtcp,buffer);
			} else {
				// common psfb
				err = yang_encode_rtcpPsfb(rtcp, buffer);
			}
		} else if (rtcp->header.type== YangRtcpType_xr) {
			err = yang_encode_rtcpXr(rtcp, buffer);
		} else {
			//rtcp = new YangRtcpCommon();
			err = yang_encode_rtcpCommon(rtcp, buffer);
		}

	}
	/** std::vector<YangRtcpCommon*>::iterator it;
	 for(it = m_rtcps.begin(); it != m_rtcps.end(); ++it) {
	 YangRtcpCommon *rtcp = *it;
	 if((err = rtcp->encode(buffer)) != Yang_Ok) {
	 return yang_error_wrap(err, "encode compound type:%d", rtcp->type());
	 }
	 }

	 clear();**/
	return err;
}
uint64_t yang_rtcpCompound_nb_bytes(){
	return 1500;
}


/**
YangRtcpCompound::YangRtcpCompound(): m_nb_bytes(0), m_data(NULL), m_nb_data(0)
{
}

YangRtcpCompound::~YangRtcpCompound()
{
   clear();
}

YangRtcpCommon* YangRtcpCompound::get_next_rtcp()
{
    if(m_rtcps.empty()) {
        return NULL;
    }
    YangRtcpCommon *rtcp = m_rtcps.back();
    m_rtcps.pop_back();
    return rtcp;
}

int32_t YangRtcpCompound::add_rtcp(YangRtcpCommon *rtcp)
{
    int32_t new_len = rtcp->nb_bytes();
    if((new_len + m_nb_bytes) > kRtcpPacketSize) {
        return yang_error_wrap(ERROR_RTC_RTCP, "overflow, new rtcp: %d, current: %d", new_len, m_nb_bytes);
    }
    m_nb_bytes += new_len;
    m_rtcps.push_back(rtcp);

    return Yang_Ok;
}

int32_t YangRtcpCompound::decode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;
    m_data = buffer->data;
    m_nb_data = buffer->size;

    while (!yang_buffer_empty(buffer)) {
        YangRtcpCommon* rtcp = NULL;
        YangRtcpHeader* header = (YangRtcpHeader*)(buffer->head);
        if (header->type == YangRtcpType_sr) {
            rtcp = new YangRtcpSR();
        } else if (header->type == YangRtcpType_rr) {
            rtcp = new YangRtcpRR();
        } else if (header->type == YangRtcpType_rtpfb) {
            if(1 == header->rc) {
                //nack
                rtcp = new YangRtcpNack();
            } else if (15 == header->rc) {
                //twcc
                rtcp = new YangRtcpTWCC();
            }
        } else if(header->type == YangRtcpType_psfb) {
            if(1 == header->rc) {
                // pli
                rtcp = new YangRtcpPli();
            } else if(2 == header->rc) {
                //sli
                //rtcp = new YangRtcpSli();
            } else if(3 == header->rc) {
                //rpsi
                rtcp = new YangRtcpRpsi();
            } else {
                // common psfb
                rtcp = new YangRtcpPsfb();
            }
        } else if(header->type == YangRtcpType_xr) {
            rtcp = new YangRtcpXr();
        } else {
            rtcp = new YangRtcpCommon();
        }

        if(Yang_Ok != (err = rtcp->decode(buffer))) {
            yang_delete(rtcp);

            // @doc https://tools.ietf.org/html/rfc3550#section-6.4.2
            // An empty RR packet (RC = 0) MUST be put at the head of a compound
            // RTCP packet when there is no data transmission or reception to
            // report. e.g. {80 c9 00 01 00 00 00 01}
            if (ERROR_RTC_RTCP_EMPTY_RR == err) {
                //srs_freep(err);
                continue;
            }

            return yang_error_wrap(err, "decode rtcp type=%u rc=%u", header->type, header->rc);
        }

        m_rtcps.push_back(rtcp);
    }

    return err;
}

uint64_t YangRtcpCompound::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpCompound::encode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;
    if(!yang_buffer_require(buffer,m_nb_bytes)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", m_nb_bytes);
    }

    std::vector<YangRtcpCommon*>::iterator it;
    for(it = m_rtcps.begin(); it != m_rtcps.end(); ++it) {
        YangRtcpCommon *rtcp = *it;
        if((err = rtcp->encode(buffer)) != Yang_Ok) {
            return yang_error_wrap(err, "encode compound type:%d", rtcp->type());
        }
    }

    clear();
    return err;
}

void YangRtcpCompound::clear()
{
    std::vector<YangRtcpCommon*>::iterator it;
    for(it = m_rtcps.begin(); it != m_rtcps.end(); ++it) {
        YangRtcpCommon *rtcp = *it;
        delete rtcp;
        rtcp = NULL;
    }
    m_rtcps.clear();
    m_nb_bytes = 0;
}

char* YangRtcpCompound::data()
{
    return m_data;
}

int32_t YangRtcpCompound::size()
{
    return m_nb_data;
}
**/

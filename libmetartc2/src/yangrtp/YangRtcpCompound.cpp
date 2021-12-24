#include <yangrtp/YangRtcpCompound.h>

#include <yangrtp/YangRtcpSR.h>
#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtcpNack.h>
#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtp/YangRtcpPli.h>
#include <yangrtp/YangRtcpSli.h>
#include <yangrtp/YangRtcpRpsi.h>

#include <yangutil/sys/YangLog.h>
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
    m_data = buffer->data();
    m_nb_data = buffer->size();

    while (!buffer->empty()) {
        YangRtcpCommon* rtcp = NULL;
        YangRtcpHeader* header = (YangRtcpHeader*)(buffer->head());
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
                rtcp = new YangRtcpSli();
            } else if(3 == header->rc) {
                //rpsi
                rtcp = new YangRtcpRpsi();
            } else {
                // common psfb
                rtcp = new YangRtcpPsfbCommon();
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
    if(!buffer->require(m_nb_bytes)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", m_nb_bytes);
    }

    vector<YangRtcpCommon*>::iterator it;
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
    vector<YangRtcpCommon*>::iterator it;
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

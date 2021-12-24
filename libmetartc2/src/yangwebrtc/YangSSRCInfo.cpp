
#include <yangwebrtc/YangSSRCInfo.h>
#include <yangutil/sys/YangLog.h>
#include <sstream>
#define kCRLF  "\r\n"
YangSSRCInfo::YangSSRCInfo()
{
    m_ssrc = 0;
}

YangSSRCInfo::YangSSRCInfo(uint32_t ssrc, std::string cname, std::string stream_id, std::string track_id)
{
    m_ssrc = ssrc;
    m_cname = cname;
    m_msid = stream_id;
    m_msid_tracker = track_id;
    m_mslabel = m_msid;
    m_label = m_msid_tracker;
}

YangSSRCInfo::~YangSSRCInfo()
{
}

int32_t YangSSRCInfo::encode(std::stringstream& os)
{
    int32_t err = Yang_Ok;

    if (m_ssrc == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrc");
    }

    os << "a=ssrc:" << m_ssrc << " cname:" << m_cname << kCRLF;
    if (!m_msid.empty()) {
        os << "a=ssrc:" << m_ssrc << " msid:" << m_msid;
        if (!m_msid_tracker.empty()) {
            os << " " << m_msid_tracker;
        }
        os << kCRLF;
    }
    if (!m_mslabel.empty()) {
        os << "a=ssrc:" << m_ssrc << " mslabel:" << m_mslabel << kCRLF;
    }
    if (!m_label.empty()) {
        os << "a=ssrc:" << m_ssrc << " label:" << m_label << kCRLF;
    }

    return err;
}

YangSSRCGroup::YangSSRCGroup()
{
}

YangSSRCGroup::~YangSSRCGroup()
{
}

YangSSRCGroup::YangSSRCGroup(const std::string& semantic, const std::vector<uint32_t>& ssrcs)
{
    m_semantic = semantic;
    m_ssrcs = ssrcs;
}

int32_t YangSSRCGroup::encode(std::ostringstream& os)
{
    int32_t err = Yang_Ok;

    if (m_semantic.empty()) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid semantics");
    }

    if (m_ssrcs.size() == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrcs");
    }

    os << "a=ssrc-group:" << m_semantic;
    for (int32_t i = 0; i < (int)m_ssrcs.size(); i++) {
        os << " " << m_ssrcs[i];
    }

    return err;
}

#ifndef SRC_YANGWEBRTC_YANGSSRCINFO_H_
#define SRC_YANGWEBRTC_YANGSSRCINFO_H_
#include <stdint.h>
#include <string>
#include <vector>
class YangSSRCInfo
{
public:
    YangSSRCInfo();
    YangSSRCInfo(uint32_t ssrc, std::string cname, std::string stream_id, std::string track_id);
    virtual ~YangSSRCInfo();
public:
    int32_t encode(std::stringstream& os);
public:
    uint32_t m_ssrc;
    std::string m_cname;
    std::string m_msid;
    std::string m_msid_tracker;
    std::string m_mslabel;
    std::string m_label;
};

class YangSSRCGroup
{
public:
    YangSSRCGroup();
    YangSSRCGroup(const std::string& usage, const std::vector<uint32_t>& ssrcs);
    virtual ~YangSSRCGroup();
public:
    int32_t encode(std::ostringstream& os);
public:
    // e.g FIX, FEC, SIM.
    std::string m_semantic;
    // SSRCs of this type.
    std::vector<uint32_t> m_ssrcs;
};
#endif /* SRC_YANGWEBRTC_YANGSSRCINFO_H_ */

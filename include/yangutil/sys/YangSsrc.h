#ifndef YANGWEBRTC_YANGRTCUTIL_H_
#define YANGWEBRTC_YANGRTCUTIL_H_
#include <string>
#include <string.h>
#include <vector>
#include <stdint.h>

using namespace std;

class YangRtcSSRCGenerator
{
private:
    static YangRtcSSRCGenerator* _instance;
private:
    uint32_t ssrc_num;
private:
    YangRtcSSRCGenerator();
    virtual ~YangRtcSSRCGenerator();
public:
    static YangRtcSSRCGenerator* instance();
    uint32_t generate_ssrc();
};

uint32_t yang_crc32_ieee(const void* buf, int32_t size, uint32_t previous);

#endif /* YANGWEBRTC_YANGRTCUTIL_H_ */

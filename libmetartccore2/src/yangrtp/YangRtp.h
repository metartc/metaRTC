
#ifndef YANGWEBRTC_YANGRTCRTP_H_
#define YANGWEBRTC_YANGRTCRTP_H_


#ifdef __cplusplus
extern "C"{
#include <yangrtp/YangCRtp.h>
}
struct YangSeqCompareLess {
    bool operator()(const uint16_t pre_value, const uint16_t value) const {
        return yang_rtp_seq_distance(pre_value, value) > 0;
    }
};
#else
#include <yangrtp/YangCRtp.h>
#endif
// For map to compare the sequence of RTP.





#endif /* YANGWEBRTC_YANGRTCRTP_H_ */

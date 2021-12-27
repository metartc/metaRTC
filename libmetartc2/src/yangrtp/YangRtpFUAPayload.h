#ifndef SRC_YANGRTP_YANGRTPFUAPAYLOAD_H_
#define SRC_YANGRTP_YANGRTPFUAPAYLOAD_H_
#include <yangrtp/YangRtp.h>
// FU-A, for one NALU with multiple fragments.
// With more than one payload.
class YangRtpFUAPayload : public IYangRtpPayloader
{
public:
    // The NRI in NALU type.
    YangAvcNaluType m_nri;
    // The FUA header.
    bool m_start;
    bool m_end;
    YangAvcNaluType m_nalu_type;
    // The NALU samples, we manage the samples.
    // @remark We only refer to the memory, user must free its bytes.
    std::vector<YangSample*> m_nalus;
public:
    YangRtpFUAPayload();
    virtual ~YangRtpFUAPayload();
// interface ISrsRtpPayloader
public:
     uint64_t nb_bytes();
     int32_t encode(YangBuffer* buf);
     int32_t decode(YangBuffer* buf);
     IYangRtpPayloader* copy();
     int32_t nb_size(){return 0;};
};
#endif /* SRC_YANGRTP_YANGRTPFUAPAYLOAD_H_ */

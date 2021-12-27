#ifndef SRC_YANGRTP_YANGRTPRAWNALUS_H_
#define SRC_YANGRTP_YANGRTPRAWNALUS_H_
#include <yangrtp/YangRtp.h>

// Multiple NALUs, automatically insert 001 between NALUs.
class YangRtpRawNALUs : public IYangRtpPayloader
{
private:
    // We will manage the samples, but the sample itself point32_t to the shared memory.
    std::vector<YangSample*> m_nalus;
    int32_t m_nn_bytes;
    int32_t m_cursor;
public:
    YangRtpRawNALUs();
    virtual ~YangRtpRawNALUs();
public:
    void push_back(YangSample* sample);
public:
    uint8_t skip_first_byte();
    // We will manage the returned samples, if user want to manage it, please copy it.
    int32_t read_samples(std::vector<YangSample*>& samples, int32_t packet_size);
// interface ISrsRtpPayloader
public:
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer* buf);
    virtual int32_t decode(YangBuffer* buf);
    virtual IYangRtpPayloader* copy();
    int32_t nb_size(){return 0;};
};

#endif /* SRC_YANGRTP_YANGRTPRAWNALUS_H_ */

#ifndef YANGSWRESAMPLE_H
#define YANGSWRESAMPLE_H

#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
class YangSwResample
{
public:
    YangSwResample();
    ~YangSwResample();
    void resample(const uint8_t *pin,uint32_t  pinLen,uint8_t* pout,uint32_t  *poutLen);
    int init(int32_t pchannel,int32_t pinsample,int32_t poutsample,int32_t pframesize);
private:
    struct SwrContext* swr_ctx;
    int32_t m_inSample;
    int32_t m_outSample;
    int32_t m_contextt;
    int32_t m_channel;
    int32_t m_frameSize;

    uint8_t** m_swrData;
};

#endif // YANGSWRESAMPLE_H

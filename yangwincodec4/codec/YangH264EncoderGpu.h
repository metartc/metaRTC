//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "NvCodec/nvenc.h"
#include "QsvCodec/QsvEncoder.h"
#include <yangutil/yangavinfotype.h>
#include <string>

class YangH264EncoderGpu
{
public:
    YangH264EncoderGpu& operator=(const YangH264EncoderGpu&) = delete;
    YangH264EncoderGpu(const YangH264EncoderGpu&) = delete;
    YangH264EncoderGpu(YangVideoInfo* pcontext,YangVideoEncInfo* enc);
    virtual ~YangH264EncoderGpu();

    void setCodec(std::string codec);

    bool init();
	void Destroy();

    int encode(YangFrame* pframe);

	int GetSequenceParams(uint8_t* out_buffer, int out_buffer_size);

    bool isSuported();

private:
    bool isKeyFrame(const uint8_t* data, uint32_t size);

	std::string codec_;
    uint8_t* m_outBuf;
    uint8_t* m_inBuf;
	void* nvenc_data_ = nullptr;
	QsvEncoder qsv_encoder_;
    YangVideoInfo* m_context;
    YangVideoEncInfo* m_enc;
    YangYuvType m_format;
    //ffmpeg::H264Encoder h264_encoder_;
};

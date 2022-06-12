//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangH264EncoderGpu.h"
#include <yangutil/sys/YangEndian.h>
#include <yangavutil/video/YangNalu.h>
#include <yangutil/sys/YangLog.h>
YangH264EncoderGpu::YangH264EncoderGpu(YangVideoInfo* pcontext,YangVideoEncInfo* enc)
{
	codec_ = "x264";
    m_context=pcontext;
    m_enc=enc;
    m_format=pcontext->videoEncoderFormat;
    if(m_context->videoEncHwType==YangV_Hw_Intel) codec_="h264_qsv";
    if(m_context->videoEncHwType==YangV_Hw_Nvdia) codec_="h264_nvenc";
    m_outBuf=NULL;
    m_inBuf=NULL;
}

YangH264EncoderGpu::~YangH264EncoderGpu()
{
    yang_deleteA(m_outBuf);
    yang_deleteA(m_inBuf);
    Destroy();
}

void YangH264EncoderGpu::setCodec(std::string codec)
{
	codec_ = codec;
}

bool YangH264EncoderGpu::init()
{
    yang_trace("\n>>YangEncoderGpu init");
	if (codec_ == "h264_nvenc") {
		if (nvenc_info.is_supported()) {
			nvenc_data_ = nvenc_info.create();
		}

		if (nvenc_data_ != nullptr) {
			nvenc_config nvenc_config;
            nvenc_config.codec = "h264";//
            if(m_context->videoEncoderType==Yang_VED_265)   nvenc_config.codec = "hevc";


            nvenc_config.format = m_format;//;
            nvenc_config.width = m_context->width;
            nvenc_config.height = m_context->height;
            nvenc_config.framerate = m_context->frame;
            nvenc_config.gop = m_enc->gop;
            nvenc_config.bitrate = m_context->rate*1000;
			if (!nvenc_info.init(nvenc_data_, &nvenc_config)) {
				nvenc_info.destroy(&nvenc_data_);
				nvenc_data_ = nullptr;
			}
		}
	}
	else if (codec_ == "h264_qsv") {
		if (QsvEncoder::IsSupported()) {
			QsvParams qsv_params;
			qsv_params.codec = "h264";
            qsv_params.bitrate_kbps = m_context->rate / 1000;
            qsv_params.framerate = m_context->frame;
            qsv_params.gop = m_enc->gop;
            qsv_params.width = m_context->width;
            qsv_params.height = m_context->height;
			if (!qsv_encoder_.Init(qsv_params)) {
				qsv_encoder_.Destroy();
			}
		}
	}

	return true;
}

void YangH264EncoderGpu::Destroy()
{
	if (nvenc_data_ != nullptr) {
		nvenc_info.destroy(&nvenc_data_);
		nvenc_data_ = nullptr;
	}

	if (qsv_encoder_.IsInitialized()) {
		qsv_encoder_.Destroy();
	}

}
bool YangH264EncoderGpu::isSuported(){
    return true;
}
bool YangH264EncoderGpu::isKeyFrame(const uint8_t* data, uint32_t size)
{
	if (size > 4) {
		//0x67:sps ,0x65:IDR, 0x6: SEI
		if (data[4] == 0x67 || data[4] == 0x65 ||
			data[4] == 0x6 || data[4] == 0x27) {
			return true;
		}
	}

	return false;
}

int YangH264EncoderGpu::encode(YangFrame* pframe)
{

	int frame_size = 0;
    int max_buffer_size = m_context->width * m_context->height * 4;

    if(m_outBuf==NULL) m_outBuf=new uint8_t[YANG_VIDEO_ENCODE_BUFFER_LEN*4];
	if (nvenc_data_ != nullptr) {
        //ID3D11Device* device = nvenc_info.get_device(nvenc_data_);
		ID3D11Texture2D* texture = nvenc_info.get_texture(nvenc_data_);
		ID3D11DeviceContext* context = nvenc_info.get_context(nvenc_data_);
		D3D11_MAPPED_SUBRESOURCE map;

		context->Map(texture, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE, 0, &map);
        int inLength=  m_context->width * m_context->height * 4;
        if (m_format == YangI420) {

            inLength=  m_context->width * m_context->height *3/2;
                uint8_t* y_plane_ = pframe->payload;
                for (int i = 0; i < m_context->height; i++) {
                    memcpy((uint8_t*)map.pData + map.RowPitch * i, y_plane_ + m_context->width * i, m_context->width);
                }
                uint8_t* uv_plane_ = pframe->payload + m_context->width * m_context->height;
                int h = m_context->height / 2;
                for (int i = 0; i < h; i++) {
                    memcpy((uint8_t*)map.pData + map.RowPitch * (m_context->height + i), uv_plane_ + m_context->width * i, m_context->width);
                }
        }else if(m_format == YangArgb){
            for (uint32_t y = 0; y < m_context->height; y++) {
                memcpy((uint8_t*)map.pData + y * map.RowPitch, pframe->payload + y * m_context->width * 4, m_context->width * 4);
            }
        }
		context->Unmap(texture, D3D11CalcSubresource(0, 0, 1));

        frame_size = nvenc_info.encode_texture(nvenc_data_, texture, m_outBuf, inLength);
	}
	else if (qsv_encoder_.IsInitialized()) {


        frame_size = qsv_encoder_.Encode(pframe->payload, m_context->width, m_context->height, m_outBuf, max_buffer_size);
	}
  if(frame_size<4) return 0;

    if(isKeyFrame(m_outBuf,frame_size)){

        YangH264NaluData2 nalu;
        YangFrame frame;
        memset(&frame,0,sizeof(YangFrame));
        frame.payload=m_outBuf;
        frame.nb=frame_size;
        if(yang_parseH264Nalu2(&frame,&nalu)==Yang_Ok){
            if(nalu.spsLen>0) yang_put_be32((char*)m_outBuf+nalu.spsPos-4,(uint32_t)nalu.spsLen);
            if(nalu.ppsLen>0) yang_put_be32((char*)m_outBuf+nalu.ppsPos-4,(uint32_t)nalu.ppsLen);
       }
        pframe->nb=frame_size;
        pframe->payload=m_outBuf;
        pframe->frametype=YANG_Frametype_I;

    }else{
        frame_size-=4;
        pframe->nb=frame_size;
        pframe->payload=m_outBuf+4;
        pframe->frametype=YANG_Frametype_P;
    }


    return frame_size;
}

int YangH264EncoderGpu::GetSequenceParams(uint8_t* out_buffer, int out_buffer_size)
{
	int size = 0;
	if (nvenc_data_ != nullptr) {
		size = nvenc_info.get_sequence_params(nvenc_data_, (uint8_t*)out_buffer, out_buffer_size);
	}
	else if (qsv_encoder_.IsInitialized()) {
		size = qsv_encoder_.GetSequenceParams((uint8_t*)out_buffer, out_buffer_size);
	}


	return size;
}

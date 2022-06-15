#ifndef QSV_ENCODER_H
#define QSV_ENCODER_H

#include "mfxvideo++.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

struct QsvParams
{
	std::string codec;
	uint32_t width;
	uint32_t height;
	uint32_t bitrate_kbps;
	uint32_t framerate;
	uint32_t gop;
};

class QsvEncoder
{
public:
	QsvEncoder & operator=(const QsvEncoder &) = delete;
	QsvEncoder(const QsvEncoder &) = delete;
	QsvEncoder();
	virtual ~QsvEncoder();

	static bool IsSupported();

	virtual bool Init(QsvParams& qsv_params);
	virtual void Destroy();
	
	virtual bool IsInitialized() const 
	{ return is_initialized_; }

    virtual int Encode(const uint8_t* nv12_image, uint32_t width, uint32_t height,
		uint8_t* out_buf, uint32_t out_buf_size);

	virtual void ForceIDR();
	virtual void SetBitrate(uint32_t bitrate_kbps);

	virtual int GetSequenceParams(uint8_t* buffer, int buffer_size);

private:
	bool InitParams(QsvParams& qsv_params);
	bool AllocateSurfaces();
	void FreeSurface();
	bool AllocateBuffer();
	void FreeBuffer();
	bool GetVideoParam();
	int  EncodeFrame(int index, uint8_t* out_buf, uint32_t out_buf_size);

	bool is_initialized_ = false;
	bool use_d3d11_ = false;
	bool use_d3d9_ = false;

	mfxIMPL                mfx_impl_;
	mfxVersion             mfx_ver_;
	MFXVideoSession        mfx_session_;
	mfxFrameAllocator      mfx_allocator_;
	mfxVideoParam          mfx_enc_params_;
	mfxVideoParam          mfx_video_params_;	
	mfxFrameAllocResponse  mfx_alloc_response_;
	mfxExtCodingOption     extended_coding_options_;
	mfxExtCodingOption2    extended_coding_options2_;
	mfxExtBuffer*          extended_buffers_[2];
	mfxEncodeCtrl          enc_ctrl_;

	std::unique_ptr<MFXVideoENCODE> mfx_encoder_;

	mfxBitstream           mfx_enc_bs_;
	std::vector<mfxU8>     bst_enc_data_;
	std::vector<mfxFrameSurface1> mfx_surfaces_;

	std::unique_ptr<mfxU8> sps_buffer_;
	std::unique_ptr<mfxU8> pps_buffer_;
	mfxU16 sps_size_ = 0;
	mfxU16 pps_size_ = 0;
};

#endif

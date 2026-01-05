//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "QsvEncoder.h"
#include "common_utils.h"
#include <Windows.h>
#include <versionhelpers.h>
#include <yangutil/sys/YangLog.h>

QsvEncoder::QsvEncoder()
	: sps_buffer_(new mfxU8[1024])
	, pps_buffer_(new mfxU8[1024])
{
	mfx_impl_ = MFX_IMPL_AUTO_ANY;
	mfx_ver_ = { {0, 1} };

	mfxIMPL impl = mfx_impl_;

	if(IsWindows8OrGreater()) {
		use_d3d11_ = true;
		impl = mfx_impl_ | MFX_IMPL_VIA_D3D11;
	}
	else {
		impl = mfx_impl_ | MFX_IMPL_VIA_D3D9;
		use_d3d9_ = true;
	}

	mfxStatus sts = MFX_ERR_NONE;
	sts = mfx_session_.Init(impl, &mfx_ver_);
	if (sts == MFX_ERR_NONE) {
		//mfx_session_.QueryVersion(&mfx_ver_);
		//mfx_session_.Close();
	}
}

QsvEncoder::~QsvEncoder()
{
	Destroy();
}

bool QsvEncoder::IsSupported()
{
	mfxVersion             mfx_ver;
	MFXVideoSession        mfx_session;
	mfxIMPL                mfx_impl;

	mfx_impl = MFX_IMPL_AUTO_ANY;
	mfx_ver = { {0, 1} };

	if (IsWindows8OrGreater()) {
		mfx_impl |= MFX_IMPL_VIA_D3D11;
	}
	else {
		mfx_impl |= MFX_IMPL_VIA_D3D9;
	}

	mfxStatus sts = MFX_ERR_NONE;
	sts = mfx_session.Init(mfx_impl, &mfx_ver);
	mfx_session.Close();
	return sts == MFX_ERR_NONE;
}

bool QsvEncoder::Init(QsvParams& qsv_params)
{
	mfxStatus sts = MFX_ERR_NONE;

	sts = Initialize(mfx_impl_, mfx_ver_, &mfx_session_, &mfx_allocator_);
	if (sts != MFX_ERR_NONE) {
		return false;
	}

	if (!InitParams(qsv_params)) {
		return false;
	}

	mfx_encoder_.reset(new MFXVideoENCODE(mfx_session_));

	sts = mfx_encoder_->Query(&mfx_enc_params_, &mfx_enc_params_);
	MSDK_IGNORE_MFX_STS(sts, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
	if (sts != MFX_ERR_NONE) {
		return false;
	}

	sts = mfx_encoder_->Init(&mfx_enc_params_);
	if (sts != MFX_ERR_NONE) {
		mfx_encoder_.reset();
		return false;
	}

	if (!AllocateSurfaces()) {
		return false;
	}

	if (!AllocateBuffer()) {
		return false;
	}

	GetVideoParam();
	is_initialized_ = true;
	return true;
}

void QsvEncoder::Destroy()
{
	if (is_initialized_) {
		FreeSurface();
		Release();
		mfx_encoder_->Close();
		is_initialized_ = false;
	}
}

bool QsvEncoder::InitParams(QsvParams& qsv_params)
{
	memset(&mfx_enc_params_, 0, sizeof(mfx_enc_params_));

	if (qsv_params.codec == "h264") {
		mfx_enc_params_.mfx.CodecId = MFX_CODEC_AVC;
		mfx_enc_params_.mfx.CodecProfile = MFX_PROFILE_AVC_BASELINE;
	}
	else if (qsv_params.codec == "hevc") {
		mfx_enc_params_.mfx.CodecId = MFX_CODEC_HEVC;
		mfx_enc_params_.mfx.CodecProfile = MFX_PROFILE_HEVC_MAIN;
	}
	else {
		return false;
	}

	mfx_enc_params_.mfx.GopOptFlag = MFX_GOP_STRICT;
	mfx_enc_params_.mfx.NumSlice = 1;
	// MFX_TARGETUSAGE_BEST_SPEED; MFX_TARGETUSAGE_BALANCED
	mfx_enc_params_.mfx.TargetUsage = MFX_TARGETUSAGE_BEST_SPEED;
	mfx_enc_params_.mfx.FrameInfo.FrameRateExtN = qsv_params.framerate;
	mfx_enc_params_.mfx.FrameInfo.FrameRateExtD = 1;
	mfx_enc_params_.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
	mfx_enc_params_.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	mfx_enc_params_.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
	mfx_enc_params_.mfx.FrameInfo.CropX = 0;
	mfx_enc_params_.mfx.FrameInfo.CropY = 0;
	mfx_enc_params_.mfx.FrameInfo.CropW = qsv_params.width;
	mfx_enc_params_.mfx.FrameInfo.CropH = qsv_params.height;

	mfx_enc_params_.mfx.RateControlMethod = MFX_RATECONTROL_CBR;
	mfx_enc_params_.mfx.TargetKbps = qsv_params.bitrate_kbps;
	//mfx_enc_params_.mfx.MaxKbps = param.bitrate_kbps;

	mfx_enc_params_.mfx.GopPicSize = (mfxU16)qsv_params.gop;
	mfx_enc_params_.mfx.IdrInterval = (mfxU16)qsv_params.gop;

	// Width must be a multiple of 16
	// Height must be a multiple of 16 in case of frame picture and a
	// multiple of 32 in case of field picture
	mfx_enc_params_.mfx.FrameInfo.Width = MSDK_ALIGN16(qsv_params.width);
	mfx_enc_params_.mfx.FrameInfo.Height = (MFX_PICSTRUCT_PROGRESSIVE == mfx_enc_params_.mfx.FrameInfo.PicStruct) ?
		MSDK_ALIGN16(qsv_params.height) : MSDK_ALIGN32(qsv_params.height); //MSDK_ALIGN16(param.height);

	// d3d11 or d3d9
	mfx_enc_params_.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;

	// Configuration for low latency
	mfx_enc_params_.AsyncDepth = 1;  //1 is best for low latency
	mfx_enc_params_.mfx.GopRefDist = 1; //1 is best for low latency, I and P frames only

	memset(&extended_coding_options_, 0, sizeof(mfxExtCodingOption));
	extended_coding_options_.Header.BufferId = MFX_EXTBUFF_CODING_OPTION;
	extended_coding_options_.Header.BufferSz = sizeof(mfxExtCodingOption);
	//option.RefPicMarkRep = MFX_CODINGOPTION_ON;
	extended_coding_options_.NalHrdConformance = MFX_CODINGOPTION_OFF;
	extended_coding_options_.PicTimingSEI = MFX_CODINGOPTION_OFF;
	extended_coding_options_.AUDelimiter = MFX_CODINGOPTION_OFF;
	extended_coding_options_.MaxDecFrameBuffering = 1;

	memset(&extended_coding_options2_, 0, sizeof(mfxExtCodingOption2));
	extended_coding_options2_.Header.BufferId = MFX_EXTBUFF_CODING_OPTION2;
	extended_coding_options2_.Header.BufferSz = sizeof(mfxExtCodingOption2);
	extended_coding_options2_.RepeatPPS = MFX_CODINGOPTION_OFF;

	extended_buffers_[0] = (mfxExtBuffer*)(&extended_coding_options_);
	extended_buffers_[1] = (mfxExtBuffer*)(&extended_coding_options2_);
	mfx_enc_params_.ExtParam = extended_buffers_;
	mfx_enc_params_.NumExtParam = 2;

	return true;
}

bool QsvEncoder::AllocateSurfaces()
{
	mfxStatus sts = MFX_ERR_NONE;

	// Query number of required surfaces for encoder
	mfxFrameAllocRequest enc_request;
	memset(&enc_request, 0, sizeof(mfxFrameAllocRequest));
	sts = mfx_encoder_->QueryIOSurf(&mfx_enc_params_, &enc_request);
	if (sts != MFX_ERR_NONE) {
		return false;
	}

	// This line is only required for Windows DirectX11 to ensure that surfaces can be written to by the application
	enc_request.Type |= WILL_WRITE;

	// Allocate required surfaces
	sts = mfx_allocator_.Alloc(mfx_allocator_.pthis, &enc_request, &mfx_alloc_response_);
	if (sts != MFX_ERR_NONE) {
		return false;
	}

	mfxU16 num_surfaces = mfx_alloc_response_.NumFrameActual;

	// Allocate surface headers (mfxFrameSurface1) for encoder
	mfx_surfaces_.resize(num_surfaces);
	for (int i = 0; i < num_surfaces; i++) {
		memset(&mfx_surfaces_[i], 0, sizeof(mfxFrameSurface1));
		mfx_surfaces_[i].Info = mfx_enc_params_.mfx.FrameInfo;
		mfx_surfaces_[i].Data.MemId = mfx_alloc_response_.mids[i];
		ClearYUVSurfaceVMem(mfx_surfaces_[i].Data.MemId);
	}

	return true;
}

void QsvEncoder::FreeSurface()
{
	if (mfx_alloc_response_.NumFrameActual > 0) {
		mfx_allocator_.Free(mfx_allocator_.pthis, &mfx_alloc_response_);
		memset(&mfx_alloc_response_, 0, sizeof(mfxFrameAllocResponse));
	}
}

bool QsvEncoder::AllocateBuffer()
{
	mfxStatus sts = MFX_ERR_NONE;
	mfxVideoParam param;
	memset(&param, 0, sizeof(mfxVideoParam));
	sts = mfx_encoder_->GetVideoParam(&param);
	if (sts != MFX_ERR_NONE) {
		return false;
	}

	memset(&mfx_enc_bs_, 0, sizeof(mfxBitstream));
	mfx_enc_bs_.MaxLength = param.mfx.BufferSizeInKB * 1000;
	bst_enc_data_.resize(mfx_enc_bs_.MaxLength);
	mfx_enc_bs_.Data = bst_enc_data_.data();
	return true;
}

void QsvEncoder::FreeBuffer()
{
	memset(&mfx_enc_bs_, 0, sizeof(mfxBitstream));
	bst_enc_data_.clear();
}

bool QsvEncoder::GetVideoParam()
{
	mfxExtCodingOptionSPSPPS opt;
	memset(&mfx_video_params_, 0, sizeof(mfxVideoParam));
	opt.Header.BufferId = MFX_EXTBUFF_CODING_OPTION_SPSPPS;
	opt.Header.BufferSz = sizeof(mfxExtCodingOptionSPSPPS);

	static mfxExtBuffer *extendedBuffers[1];
	extendedBuffers[0] = (mfxExtBuffer *)&opt;
	mfx_video_params_.ExtParam = extendedBuffers;
	mfx_video_params_.NumExtParam = 1;

	opt.SPSBuffer = sps_buffer_.get();
	opt.PPSBuffer = pps_buffer_.get();
	opt.SPSBufSize = 1024;
	opt.PPSBufSize = 1024;

	mfxStatus sts = mfx_encoder_->GetVideoParam(&mfx_video_params_);
	if (sts != MFX_ERR_NONE) {
		return false;
	}

	sps_size_ = opt.SPSBufSize;
	pps_size_ = opt.PPSBufSize;

	//printf("\n");
	//for (uint32_t i = 0; i < 150 && i < sps_size_; i++) {
	//	printf("%x ", sps_buffer_.get()[i]);
	//}
	//printf("\n");

	return true;
}

int QsvEncoder::Encode(const uint8_t* nv12_image, uint32_t width, uint32_t height,
	uint8_t* out_buf, uint32_t out_buf_size)
{
	if (!is_initialized_) {
		return -1;
	}

	int yuv_buf_size = width * height * 3 / 2;
    //std::shared_ptr<uint8_t> nv12_buf(new uint8_t[yuv_buf_size], std::default_delete<uint8_t[]>());

    //int stride_y = width;
    //int stride_uv = width;// (width + 1) / 2;
    //uint8_t* data_y = nv12_buf.get();
    //uint8_t* data_uv = nv12_buf.get() + width * height;

    //int ret = libyuv::ARGBToNV12(bgra_image, width * 4, data_y, stride_y, data_uv, stride_uv, width, height);
    //if (ret != 0) {
    //	return -1;
    //}

	mfxStatus sts = MFX_ERR_NONE;

	int index = GetFreeSurfaceIndex(mfx_surfaces_);  // Find free frame surface
	MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);

	// Surface locking required when read/write video surfaces
	sts = mfx_allocator_.Lock(mfx_allocator_.pthis, mfx_surfaces_[index].Data.MemId, &(mfx_surfaces_[index].Data));
	MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_LOCK_MEMORY);

	// load nv12 
	mfxU16 w, h, i, pitch;
	mfxU8 *ptr;
	mfxFrameInfo *info = &mfx_surfaces_[index].Info;
	mfxFrameData *data = &mfx_surfaces_[index].Data;

	if (info->CropH > 0 && info->CropW > 0) {
		w = info->CropW;
		h = info->CropH;
	}
	else {
		w = info->Width;
		h = info->Height;
	}

	pitch = data->Pitch;
	ptr = data->Y + info->CropX + info->CropY * data->Pitch;

	// load Y plane
	for (i = 0; i < h; i++)
        memcpy(ptr + i * pitch, nv12_image + i * width, w);

	// load UV plane
	h /= 2;
	ptr = data->UV + info->CropX + (info->CropY / 2) * pitch;
    uint8_t* data_uv = (uint8_t*)nv12_image + width * height;
    int stride_uv = width;
	for (i = 0; i < h; i++) {
		memcpy(ptr + i * pitch, data_uv + i * stride_uv, w);
	}

	sts = mfx_allocator_.Unlock(mfx_allocator_.pthis, mfx_surfaces_[index].Data.MemId, &(mfx_surfaces_[index].Data));
	MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_UNKNOWN);

	return EncodeFrame(index, out_buf, out_buf_size);
}

static void SaveFile(uint8_t* frame_data, uint32_t frame_size, bool is_h264)
{
	printf("\n");
	for (uint32_t i = 0; i < 150 && i < frame_size; i++) {
		printf("%x ", frame_data[i]);
	}
	printf("\n");
	printf("frame size: %d\n", frame_size);

	static FILE* file = NULL;
	if (!file) {
		if (is_h264) {
			file = fopen("test.h264", "wb+");
		}
		else {
			file = fopen("test.h265", "wb+");
		}
	}

	if (file) {
		fwrite(frame_data, 1, frame_size, file);
	}
}

int QsvEncoder::EncodeFrame(int index, uint8_t* out_buf, uint32_t out_buf_size)
{
	mfxSyncPoint syncp;
	mfxStatus sts = MFX_ERR_NONE;
	uint32_t frame_size = 0;

	for (;;) {
		// Encode a frame asychronously (returns immediately)
		mfxEncodeCtrl* enc_ctrl = nullptr;
		if (enc_ctrl_.FrameType) {
			enc_ctrl = &enc_ctrl_;
		}
		sts = mfx_encoder_->EncodeFrameAsync(enc_ctrl, &mfx_surfaces_[index], &mfx_enc_bs_, &syncp);
		enc_ctrl_.FrameType = 0;

		if (MFX_ERR_NONE < sts && !syncp) {  // Repeat the call if warning and no output
			if (MFX_WRN_DEVICE_BUSY == sts)
				MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call
		}
		else if (MFX_ERR_NONE < sts && syncp) {
			sts = MFX_ERR_NONE;     // Ignore warnings if output is available
			break;
		}
		else if (MFX_ERR_NOT_ENOUGH_BUFFER == sts) {
			// Allocate more bitstream buffer memory here if needed...
			break;
		}
		else {
			break;
		}
	}

	if (MFX_ERR_NONE == sts) {
		sts = mfx_session_.SyncOperation(syncp, 60000);   // Synchronize. Wait until encoded frame is ready
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		//mfx_enc_bs_.Data + mfx_enc_bs_.DataOffset, mfx_enc_bs_.DataLength
		if (mfx_enc_bs_.DataLength > 0) {
			//printf("encoder output frame size: %u \n", mfx_enc_bs_.DataLength);
			if (out_buf_size >= frame_size + sps_size_ + pps_size_) {
				//memcpy(out_buf + frame_size, sps_buffer_.get(), sps_size_);
				//frame_size += sps_size_;
				//memcpy(out_buf + frame_size, pps_buffer_.get(), pps_size_);
				//frame_size += pps_size_;

				if (mfx_enc_params_.mfx.CodecId == MFX_CODEC_AVC) {
					memcpy(out_buf + frame_size, mfx_enc_bs_.Data, mfx_enc_bs_.DataLength);
				}
				else if (mfx_enc_params_.mfx.CodecId == MFX_CODEC_HEVC) {
					memcpy(out_buf + frame_size, mfx_enc_bs_.Data, mfx_enc_bs_.DataLength);
				}

				frame_size += mfx_enc_bs_.DataLength;

				//SaveFile(out_buf, frame_size, mfx_enc_params_.mfx.CodecId == MFX_CODEC_AVC);			
			}

			mfx_enc_bs_.DataLength = 0;
		}
	}

	return frame_size;
}

void QsvEncoder::ForceIDR()
{
	if (is_initialized_) {
		enc_ctrl_.FrameType = MFX_FRAMETYPE_I | MFX_FRAMETYPE_IDR | MFX_FRAMETYPE_REF;
	}
}

void QsvEncoder::SetBitrate(uint32_t bitrate_kbps)
{
	if (mfx_encoder_) {
		mfxVideoParam old_param;
		memset(&old_param, 0, sizeof(mfxVideoParam));
		mfxStatus status = mfx_encoder_->GetVideoParam(&old_param);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);

		if (status == MFX_ERR_NONE) {
			uint32_t old_bitrate = old_param.mfx.TargetKbps;
			old_param.mfx.TargetKbps = bitrate_kbps;
			status = mfx_encoder_->Reset(&old_param);
			MSDK_IGNORE_MFX_STS(status, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
			if (status == MFX_ERR_NONE) {
                yang_trace("Reset bitrate:%u, old bitrate:%u", bitrate_kbps, old_bitrate);
			}
			else {
                yang_trace("[Reset bitrate failed, bitrate:%u, status:%d",
					old_param.mfx.TargetKbps, status);
			}
		}
		else {
            yang_trace("GetVideoParam() failed");
		}
	}
}

int QsvEncoder::GetSequenceParams(uint8_t* buffer, int buffer_size)
{
	int size = 0;

	if (is_initialized_) {
		if (sps_size_ && pps_size_) {
			if (buffer_size >= (sps_size_ + pps_size_)) {
				memcpy(buffer + size, sps_buffer_.get(), sps_size_);
				size += sps_size_;
				memcpy(buffer + size, pps_buffer_.get(), pps_size_);
				size += pps_size_;
			}
		}
	}

	return size;
}

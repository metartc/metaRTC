
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANGAVTYPE_H_
#define INCLUDE_YANGUTIL_YANGAVTYPE_H_

#include <yangutil/yangtype.h>

    // 8 = audio
#define    YangFrameTypeAudio 8
    // 9 = video
#define YangFrameTypeVideo  9
    // 18 = script data
#define kNalTypeMask       0x1F

#define YangAudioPayloadType 111
#define YangAV1PayloadType 123
#define YangH264PayloadType 106
#define YangH265PayloadType 126
#define YangMjpegPayloadType 26


#define Yang_TWCC_ID 3

#define kDefaultLowLossThreshold 0.02f
#define kDefaultHighLossThreshold 0.1f



enum{
	YANG_DATA_CHANNEL_DCEP = 50,
	YANG_DATA_CHANNEL_STRING = 51,
	YANG_DATA_CHANNEL_BINARY = 53,
	YANG_DATA_CHANNEL_STRING_EMPTY = 56,
	YANG_DATA_CHANNEL_BINARY_EMPTY = 57
};

typedef enum YangYuvType{
    YangYuy2,
	YangI420,
	YangYv12,
	YangNv12,
	YangNv21,
	YangRgb,
    YangArgb,
    YangBgra,
	YangP010,
    YangP016
}YangColorSpace;

typedef enum YangRatate{
	Yang_Rotate0 = 0,      // No rotation.
	Yang_Rotate90 = 90,    // Rotate 90 degrees clockwise.
	Yang_Rotate180 = 180,  // Rotate 180 degrees.
	Yang_Rotate270 = 270,
}YangRatate;
/**
 * the level for avc/h.264.
 * @see Annex A Profiles and levels, ISO_IEC_14496-10-AVC-2003.pdf, page 207.
 */
enum YangAvcLevel
{
    YangAvcLevelReserved = 0,

    YangAvcLevel_1 = 10,
    YangAvcLevel_11 = 11,
    YangAvcLevel_12 = 12,
    YangAvcLevel_13 = 13,
    YangAvcLevel_2 = 20,
    YangAvcLevel_21 = 21,
    YangAvcLevel_22 = 22,
    YangAvcLevel_3 = 30,
    YangAvcLevel_31 = 31,
    YangAvcLevel_32 = 32,
    YangAvcLevel_4 = 40,
    YangAvcLevel_41 = 41,
    YangAvcLevel_5 = 50,
    YangAvcLevel_51 = 51,
};

/**
 * Table 7-6 – Name association to slice_type
 * ISO_IEC_14496-10-AVC-2012.pdf, page 105.
 */
enum YangAvcSliceType
{
    YangAvcSliceTypeP   = 0,
    YangAvcSliceTypeB   = 1,
    YangAvcSliceTypeI   = 2,
    YangAvcSliceTypeSP  = 3,
    YangAvcSliceTypeSI  = 4,
    YangAvcSliceTypeP1  = 5,
    YangAvcSliceTypeB1  = 6,
    YangAvcSliceTypeI1  = 7,
    YangAvcSliceTypeSP1 = 8,
    YangAvcSliceTypeSI1 = 9,
};


typedef enum
{
    // Unspecified
    YangAvcNaluTypeReserved = 0,
    YangAvcNaluTypeForbidden = 0,

    // Coded slice of a non-IDR picture slice_layer_without_partitioning_rbsp( )
    YangAvcNaluTypeNonIDR = 1,
    // Coded slice data partition A slice_data_partition_a_layer_rbsp( )
    YangAvcNaluTypeDataPartitionA = 2,
    // Coded slice data partition B slice_data_partition_b_layer_rbsp( )
    YangAvcNaluTypeDataPartitionB = 3,
    // Coded slice data partition C slice_data_partition_c_layer_rbsp( )
    YangAvcNaluTypeDataPartitionC = 4,
    // Coded slice of an IDR picture slice_layer_without_partitioning_rbsp( )
    YangAvcNaluTypeIDR = 5,
    // Supplemental enhancement information (SEI) sei_rbsp( )
    YangAvcNaluTypeSEI = 6,
    // Sequence parameter set seq_parameter_set_rbsp( )
    YangAvcNaluTypeSPS = 7,
    // Picture parameter set pic_parameter_set_rbsp( )
    YangAvcNaluTypePPS = 8,
    // Access unit delimiter access_unit_delimiter_rbsp( )
    YangAvcNaluTypeAccessUnitDelimiter = 9,
    // End of sequence end_of_seq_rbsp( )
    YangAvcNaluTypeEOSequence = 10,
    // End of stream end_of_stream_rbsp( )
    YangAvcNaluTypeEOStream = 11,
    // Filler data filler_data_rbsp( )
    YangAvcNaluTypeFilterData = 12,
    // Sequence parameter set extension seq_parameter_set_extension_rbsp( )
    YangAvcNaluTypeSPSExt = 13,
    // Prefix NAL unit prefix_nal_unit_rbsp( )
    YangAvcNaluTypePrefixNALU = 14,
    // Subset sequence parameter set subset_seq_parameter_set_rbsp( )
    YangAvcNaluTypeSubsetSPS = 15,
    // Coded slice of an auxiliary coded picture without partitioning slice_layer_without_partitioning_rbsp( )
    YangAvcNaluTypeLayerWithoutPartition = 19,
    // Coded slice extension slice_layer_extension_rbsp( )
    YangAvcNaluTypeCodedSliceExt = 20,
}YangAvcNaluType;

enum YangAvcProfile
{
    YangAvcProfileReserved = 0,

    // @see ffmpeg, libavcodec/avcodec.h:2713
    YangAvcProfileBaseline = 66,
    // FF_PROFILE_H264_CONSTRAINED  (1<<9)  // 8+1; constraint_set1_flag
    // FF_PROFILE_H264_CONSTRAINED_BASELINE (66|FF_PROFILE_H264_CONSTRAINED)
    YangAvcProfileConstrainedBaseline = 578,
    YangAvcProfileMain = 77,
    YangAvcProfileExtended = 88,
    YangAvcProfileHigh = 100,
    YangAvcProfileHigh10 = 110,
    YangAvcProfileHigh10Intra = 2158,
    YangAvcProfileHigh422 = 122,
    YangAvcProfileHigh422Intra = 2170,
    YangAvcProfileHigh444 = 144,
    YangAvcProfileHigh444Predictive = 244,
    YangAvcProfileHigh444Intra = 2192,
};

struct YangRect{
    short x;
    short y;
    short w;
    short h;
};
struct YangColor{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
typedef struct  {
	int32_t vpsLen;
	int32_t spsLen;
	int32_t ppsLen;
	uint8_t vps[128];
	uint8_t sps[128];
	uint8_t pps[64];
}YangH2645Conf;

typedef struct  {
	uint8_t buffer[128];
	int32_t bufLen;
}YangRtmpMeta;

typedef struct  {
	int32_t isInit;
	 YangH2645Conf mp4Meta;
	//YangRtmpMeta flvMeta;
	 YangRtmpMeta livingMeta;
}YangVideoMeta;


enum YangVideoHwType{
	Yang_Hw_Soft,
	YangV_Hw_Intel,
	YangV_Hw_Nvdia,
	YangV_Hw_Android
};

struct YangMessage{
	int32_t mediaType;
	int32_t nb;
	int64_t timestamp;
	char* payload;
};

typedef struct {
	int32_t nb;
	char* bytes;
}YangSample;

YangSample* yang_sample_copy(YangSample* src);



#include <yangutil/yangavctype.h>
#ifdef __cplusplus
extern "C"{
#include <yangutil/yangframebuffer.h>
}
#else
#include <yangutil/yangframebuffer.h>
#endif
#ifdef __cplusplus
class YangBufferManager{
public:
	YangBufferManager();
	YangBufferManager(int32_t num,int32_t bufsize);
	virtual ~YangBufferManager();
	void init(int32_t num,int32_t bufsize);
	uint8_t* getBuffer();
private:
	uint8_t* m_cache;
	int32_t m_curindex;
	int32_t m_unitsize;
	int32_t m_size;
};
#endif
#endif /* INCLUDE_YANGUTIL_YANGAVTYPE_H_ */

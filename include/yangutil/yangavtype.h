#ifndef INCLUDE_YANGUTIL_YANGAVTYPE_H_
#define INCLUDE_YANGUTIL_YANGAVTYPE_H_
#include <stdio.h>
#include <stdint.h>
#define YANG_Frametype_Spspps 9
#define YANG_Frametype_I 1
#define YANG_Frametype_P 0

#define kNalTypeMask       0x1F
enum YangYuvType{
    YangYuy2,
	YangI420,
	YangNv12,
    YangYv12,
	YangRgb,
    YangArgb,
    YangBgra,
	YangP010,
    YangP016
};

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


enum YangAvcNaluType
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
};

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
struct YangH2645Conf {
	int32_t vpsLen;
	int32_t spsLen;
	int32_t ppsLen;
	uint8_t vps[128];
	uint8_t sps[128];
	uint8_t pps[64];
};
struct YangRtmpMeta {
	uint8_t buffer[128];
	int32_t bufLen;
};
struct YangVideoMeta {
	int32_t isInit;
	YangH2645Conf mp4Meta;
	//YangRtmpMeta flvMeta;
	YangRtmpMeta livingMeta;
};

enum YangAudioEncDecType{
	Yang_AED_AAC,
	Yang_AED_MP3,
	Yang_AED_SPEEX,
	Yang_AED_OPUS
};
enum YangVideoEncDecType{
	Yang_VED_264,
	Yang_VED_265,
	Yang_VED_vp9
};
enum YangVideoHwType{
	Yang_Hw_Soft,
	YangV_Hw_Intel,
	YangV_Hw_Nvdia,
	YangV_Hw_Android
};
struct YangAudioParam{
	YangAudioEncDecType encode;
	int32_t sample;
	int32_t channel;
    int32_t audioClock;
};

struct YangVideoParam{
	YangVideoEncDecType encode;
	int32_t videoClock;

};
struct YangFrame{
	int32_t mediaType;
	int32_t uid;
	int32_t frametype;
	int32_t nb;
	int64_t timestamp;
	uint8_t* payload;
};

void yang_frame_copy( YangFrame* src,YangFrame* dst);
void yang_frame_copy_buffer( YangFrame* src,YangFrame* dst);
void yang_frame_copy_nobuffer(YangFrame* src,YangFrame* dst);
//void yang_frame_init(YangFrame* frame);

struct YangMessage{
	int32_t mediaType;
	int32_t nb;
	int64_t timestamp;

	char* payload;
	YangMessage(){
		mediaType=0;
		nb=0;
		timestamp=0;
		payload=NULL;
	}

};
struct YangSample{
	int32_t nb;
	char* bytes;
};
YangSample* yang_sample_copy(YangSample* src);

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

#endif /* INCLUDE_YANGUTIL_YANGAVTYPE_H_ */

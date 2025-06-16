//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANGAVINFO_H_
#define INCLUDE_YANGUTIL_YANGAVINFO_H_

#include <yangutil/yangtype.h>

#define YANG_Frametype_Spspps 9
#define YANG_Frametype_I 1
#define YANG_Frametype_P 0
#define YANG_Frametype_B 7
#define YANG_Frametype_Audio 10

#define Yang_MJPEG_Header 0x37

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


typedef enum YangHevcNaluType
{
	YANG_NAL_UNIT_CODED_SLICE_TRAIL_N = 0,
	YANG_NAL_UNIT_CODED_SLICE_TRAIL_R, //1
	YANG_NAL_UNIT_CODED_SLICE_TSA_N,   //2
	YANG_NAL_UNIT_CODED_SLICE_TLA,     //3
	YANG_NAL_UNIT_CODED_SLICE_STSA_N,  //4
	YANG_NAL_UNIT_CODED_SLICE_STSA_R,  //5
	YANG_NAL_UNIT_CODED_SLICE_RADL_N,  //6
	YANG_NAL_UNIT_CODED_SLICE_DLP,     //7
	YANG_NAL_UNIT_CODED_SLICE_RASL_N,  //8
	YANG_NAL_UNIT_CODED_SLICE_TFD,     //9
	YANG_NAL_UNIT_RESERVED_10,
	YANG_NAL_UNIT_RESERVED_11,
	YANG_NAL_UNIT_RESERVED_12,
	YANG_NAL_UNIT_RESERVED_13,
	YANG_NAL_UNIT_RESERVED_14,
	YANG_NAL_UNIT_RESERVED_15,
	YANG_NAL_UNIT_CODED_SLICE_BLA,      //16
	YANG_NAL_UNIT_CODED_SLICE_BLANT,    //17
	YANG_NAL_UNIT_CODED_SLICE_BLA_N_LP, //18
	YANG_NAL_UNIT_CODED_SLICE_IDR,      //19
	YANG_NAL_UNIT_CODED_SLICE_IDR_N_LP, //20
	YANG_NAL_UNIT_CODED_SLICE_CRA,      //21
	YANG_NAL_UNIT_VPS=32,                   //32
	YANG_NAL_UNIT_SPS,                   // 33
	YANG_NAL_UNIT_PPS,                   //34
	YANG_NAL_UNIT_ACCESS_UNIT_DELIMITER, //35
	YANG_NAL_UNIT_EOS,                   //36
	YANG_NAL_UNIT_EOB,                   //37
	YANG_NAL_UNIT_FILLER_DATA,           //38
	YANG_NAL_UNIT_SEI ,                  //39Prefix SEI
	YANG_NAL_UNIT_SEI_SUFFIX,            //40Suffix SEI
	YANG_NAL_UNIT_RESERVED_41,
	YANG_NAL_UNIT_RESERVED_42,
	YANG_NAL_UNIT_RESERVED_43,
	YANG_NAL_UNIT_RESERVED_44,
	YANG_NAL_UNIT_RESERVED_45,
	YANG_NAL_UNIT_RESERVED_46,
	YANG_NAL_UNIT_RESERVED_47,
	YANG_NAL_UNIT_UNSPECIFIED_48,
	YANG_NAL_UNIT_UNSPECIFIED_49,
	YANG_NAL_UNIT_UNSPECIFIED_50,
	YANG_NAL_UNIT_UNSPECIFIED_51,
	YANG_NAL_UNIT_UNSPECIFIED_52,
	YANG_NAL_UNIT_UNSPECIFIED_53,
	YANG_NAL_UNIT_UNSPECIFIED_54,
	YANG_NAL_UNIT_UNSPECIFIED_55,
	YANG_NAL_UNIT_UNSPECIFIED_56,
	YANG_NAL_UNIT_UNSPECIFIED_57,
	YANG_NAL_UNIT_UNSPECIFIED_58,
	YANG_NAL_UNIT_UNSPECIFIED_59,
	YANG_NAL_UNIT_UNSPECIFIED_60,
	YANG_NAL_UNIT_UNSPECIFIED_61,
	YANG_NAL_UNIT_UNSPECIFIED_62,
	YANG_NAL_UNIT_UNSPECIFIED_63,
	YANG_NAL_UNIT_INVALID,
}YangHevcNaluType;

//for nalu data first byte
//#define YANG_HEVC_NALU_TYPE(code) (YangHevcNaluType)((code & 0x7E)>>1)
#define YANG_HEVC_NALU_TYPE(code) (YangHevcNaluType)((code >> 1) & 0x3F)
#define H265_kFuA 49
#define H265_kStapA 48

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

typedef enum YangAudioCodec{
	Yang_AED_AAC,
	Yang_AED_MP3,
	Yang_AED_SPEEX,
	Yang_AED_OPUS,
	Yang_AED_PCMA,
	Yang_AED_PCMU
}YangAudioCodec;

typedef enum YangVideoCodec{
	Yang_VED_H264,
	Yang_VED_H265,
	Yang_VED_AV1,
	Yang_VED_VP8,
	Yang_VED_VP9,
	Yang_VED_MJPEG
}YangVideoCodec;

//typedef enum  {
//	YangRecvonly, YangSendonly, YangSendrecv
//}YangRtcDirection;

typedef enum {
    Yang_Conn_State_New,
    Yang_Conn_State_Connecting,
    Yang_Conn_State_Connected,
    Yang_Conn_State_Disconnected,
    Yang_Conn_State_Failed,
    Yang_Conn_State_Closed
}YangRtcConnectionState;

typedef enum{
	YangMediaAudio,
	YangMediaVideo
}YangMediaTrack;

typedef enum{
	YangSendrecv,
	YangSendonly,
	YangRecvonly,
	YangInactive
}YangRtcDirection;


typedef enum YangRequestType {
	Yang_Req_Sendkeyframe,
	Yang_Req_HighLostPacketRate,
	Yang_Req_LowLostPacketRate,
	Yang_Req_Connected,
	Yang_Req_Disconnected
}YangRequestType;

typedef enum YangRtcMessageType{
	YangRTC_Decoder_Error
}YangRtcMessageType;


typedef enum{
	YangIceModeFull,
	YangIceModeLite
}YangIceMode;

typedef enum{
	YangTransportAll,
	YangTransportRelay
}YangIceTransportPolicy;

typedef enum{
	YangIceHost,
	YangIceStun,
	YangIceTurn
}YangIceCandidateType;

typedef enum {
	YangIceNew,
    YangIceSuccess,
    YangIceFail
}YangIceCandidateState;

typedef enum {
	YangIceGatherNew,
	YangIceGathering,
    YangIceGatherComplete
} YangIceGatheringState;

typedef struct{
	int32_t uid;
	int32_t mediaType;
	int32_t frametype;
	int32_t nb;
	int64_t pts;
	int64_t dts;
	uint8_t* payload;
}YangFrame;


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



typedef struct YangAudioInfo {

	yangbool enableMono;
	yangbool enableAec;
	yangbool enableAudioFec;
	yangbool enableAudioHeader;

	int32_t sample;
	int32_t frameSize;
	int32_t bitrate;
	int32_t channel;

	int32_t echoPath;
	int32_t aecBufferFrames;
	int32_t audioCacheSize;
	int32_t audioCacheNum;
	int32_t audioPlayCacheNum;

	int32_t audioEncoderType;
	int32_t audioDecoderType;
	int32_t audioPlayType;

	int32_t aIndex;
	int32_t aSubIndex;
}YangAudioInfo;

typedef struct YangVideoInfo {
	int32_t width; //= 800
	int32_t height; //= 600
	int32_t outWidth;
	int32_t outHeight;
	int32_t rate; // 512
	int32_t frame; //25
	int32_t rotate; // 16
	int32_t bitDepth;

	int32_t videoCacheNum;
	int32_t evideoCacheNum;
	int32_t videoPlayCacheNum;

	YangColorSpace videoCaptureFormat;
	YangColorSpace videoEncoderFormat;
	YangColorSpace videoDecoderFormat;

	int32_t videoEncoderType;
	int32_t videoDecoderType;
	int32_t videoEncHwType;
	int32_t videoDecHwType;
	int32_t vIndex;
}YangVideoInfo;

typedef struct YangVideoEncInfo {
	yangbool createMeta;
	int32_t preset;
	int32_t level_idc;
	int32_t profile;
	int32_t keyint_max;
	int32_t enc_threads;
	int32_t gop;

}YangVideoEncInfo;

typedef struct YangSysInfo {
	yangbool enableLogFile;
	YangIpFamilyType familyType;

	int32_t mediaServer;
	int32_t httpPort;
	int32_t transType;
	int32_t logLevel;
	char whipUrl[128];
	char whepUrl[128];
}YangSysInfo;

typedef struct YangRtcInfo {
		yangbool isControlled;
		yangbool enableSdpCandidate;

		int32_t  sessionTimeout;
		int32_t  iceCandidateType;

		YangIceTransportPolicy iceTransportPolicy;

		int32_t  iceServerPort;

		int32_t  rtcSocketProtocol;
		int32_t  turnSocketProtocol;

		int32_t  rtcPort;
		int32_t  rtcLocalPort;

		int32_t turnReqInterval;
		int32_t maxTurnWaitTime;

		char iceServerIP[64];
		char iceUserName[32];
		char icePassword[64];
}YangRtcInfo;

typedef struct{
	yangbool enableMqttTls;
	int32_t  mqttPort;
	int32_t  maxReconnectTimes;
	int32_t  reconnectIntervalTime;
	char mqttServerIP[32];
	char mqttUserName[32];
	char mqttPassword[64];
}YangMqttInfo;

typedef struct YangAVInfo{
	YangSysInfo sys;
	YangAudioInfo audio;
	YangVideoInfo video;
	YangVideoEncInfo enc;
	YangRtcInfo rtc;
	YangMqttInfo* mqtt;
}YangAVInfo;

typedef struct {
	enum YangAudioCodec encode;
	int32_t sample;
	int32_t channel;
    int32_t audioClock;
    int32_t fec;
}YangAudioParam;

typedef struct  {
	enum YangVideoCodec encode;
	int32_t videoClock;
}YangVideoParam;

typedef struct{
 void* context;
 int32_t (*sendRtcMessage)(void* context,int puid,YangRtcMessageType mess);
}YangSendRtcMessage;

typedef struct{
	YangIpFamilyType familyType;
	int32_t serverPort;
	uint32_t stunIp;
	int32_t stunPort;
	uint32_t relayIp;
	int32_t relayPort;
	char username[64];
	char password[64];
	char serverIp[64];
}YangIceServer;

typedef struct{
	void* context;
	void (*sslAlert)(void* context,int32_t uid,char* type,char* desc);
}YangSslCallback;

typedef struct {
	 void* context;
	 void (*receiveAudio)(void* context,YangFrame *audioFrame);
	 void (*receiveVideo)(void* context,YangFrame *videoFrame);
	 void (*receiveMsg)(void* context,YangFrame *videoFrame);
}YangReceiveCallback;

typedef struct{
	void* context;
	void (*onIceStateChange)(void* context,int32_t uid,YangIceCandidateState iceState);
	void (*onConnectionStateChange)(void* context, int32_t uid,YangRtcConnectionState connectionState);
	void (*onIceCandidate)(void* context, int32_t uid,char* sdp);
	void (*onIceGatheringState)(void* context, int32_t uid,YangIceGatheringState gatherState);
}YangIceCallback;

typedef struct{
	void* context;
	void (*setMediaConfig)(void* context,int32_t puid,YangAudioParam* audio,YangVideoParam* video);
	void (*sendRequest)(void* context,int32_t puid,uint32_t ssrc,YangRequestType req);
}YangRtcCallback;

typedef struct  {
	YangSslCallback sslCallback;
	YangRtcCallback rtcCallback;
	YangReceiveCallback recvCallback;
	YangIceCallback iceCallback;

}YangPeerCallback;

typedef struct{
	int32_t enableAudioFec;
	int32_t sample;
	int32_t channel;

	YangAudioCodec  audioEncoderType;
}YangPushAudioInfo;

typedef struct{
	int32_t width;
	int32_t height;
	int32_t fps;
	YangVideoCodec  videoEncoderType;
}YangPushVideoInfo;

typedef struct{
	int32_t uid;
	int32_t userId;
	int32_t remotePort;
	int32_t mediaServer;
	YangIceMode iceMode;
	YangIpFamilyType familyType;
	YangRtcDirection direction;
	char remoteIp[64];
	YangRtcInfo rtc;
	YangPushAudioInfo pushAudio;
	YangPushVideoInfo pushVideo;
}YangPeerInfo;

typedef struct{
	void* conn;
	YangPeerInfo peerInfo;
	YangPeerCallback peerCallback;
}YangPeer;



YangSample* yang_sample_copy(YangSample* src);

#endif /* INCLUDE_YANGUTIL_YANGAVINFO_H_ */

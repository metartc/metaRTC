
/*
 * YangTsMux.h
 *
 *  Created on: 2020年8月7日
 *      Author: yang
 */

#ifndef YANGTS_YANGTSMUXER_H_
#define YANGTS_YANGTSMUXER_H_
#include "stdint.h"
#include <vector>
#include <map>

#include "YangTsBuffer.h"
#include "YangTsPacket.h"
#include "YangTsPid.h"
using namespace std;

/**
static const int32_t DEFAULT_PCR_PID = 4097;
static const int32_t DEFAULT_PMT_PID = 256;

static const uint8_t SEQUENCE_END_CODE = 0xb7;
static const uint8_t ISO_11172_END_CODE = 0xb9;
static const uint8_t PACK_START_CODE = 0xba;
static const uint8_t SYSTEM_HEADER_START_CODE = 0xbb;
static const uint8_t PES_PROGRAM_STREAM_MAP = 0xbc;
static const uint8_t PES_PRIVATE_DATA1 = 0xbd;
static const uint8_t PADDING_STREAM = 0xbe;
static const uint8_t PES_PRIVATE_DATA2 = 0xbf;
static const uint8_t PROGRAM_STREAM_DIRECTORY = 0xff;

static const uint8_t PES_AUDIO_ID = 0xc0;
static const uint8_t PES_VIDEO_ID = 0xe1;
static const uint8_t PES_VC1_ID = 0xfd;

static const uint8_t DVB_SUBT_DESCID = 0x59;

static const uint8_t STREAM_TYPE_VIDEO_MPEG1 = 0x01;
static const uint8_t STREAM_TYPE_VIDEO_MPEG2 = 0x02;
static const uint8_t STREAM_TYPE_PRIVATE_SECTION = 0x05;
static const uint8_t STREAM_TYPE_PRIVATE_DATA = 0x06;
static const uint8_t STREAM_TYPE_VIDEO_MPEG4 = 0x10;
static const uint8_t STREAM_TYPE_VIDEO_H264 = 0x1b;
static const uint8_t STREAM_TYPE_VIDEO_MVC = 0x20;
static const uint8_t STREAM_TYPE_VIDEO_H265 = 0x24;
static const uint8_t STREAM_TYPE_VIDEO_VC1 = 0xea;

static const uint8_t STREAM_TYPE_AUDIO_MPEG1 = 0x03;
static const uint8_t STREAM_TYPE_AUDIO_MPEG2 = 0x04;
static const uint8_t STREAM_TYPE_AUDIO_AAC = 0x0f;
static const uint8_t STREAM_TYPE_AUDIO_AAC_RAW = 0x11;
static const uint8_t STREAM_TYPE_AUDIO_DTS = 0x82;  // 0x8a

static const uint8_t STREAM_TYPE_AUDIO_LPCM = 0x80;
static const uint8_t STREAM_TYPE_AUDIO_AC3 = 0x81;
static const uint8_t STREAM_TYPE_AUDIO_EAC3 = 0x84;
static const uint8_t STREAM_TYPE_AUDIO_EAC3_ATSC = 0x87;
static const uint8_t STREAM_TYPE_AUDIO_EAC3_TRUE_HD = 0x83;
static const uint8_t STREAM_TYPE_AUDIO_DTS_HD = 0x85;
static const uint8_t STREAM_TYPE_AUDIO_DTS_HD_MASTER_AUDIO = 0x86;

static const uint8_t STREAM_TYPE_AUDIO_EAC3_SECONDARY = 0xA1;
static const uint8_t STREAM_TYPE_AUDIO_DTS_HD_SECONDARY = 0xA2;

static const uint8_t STREAM_TYPE_AUDIO_VC9 = 0x88;
static const uint8_t STREAM_TYPE_AUDIO_OPUS = 0x89;
static const uint8_t STREAM_TYPE_SUB_PGS = 0x90;

static const uint8_t STREAM_TYPE_SUBTITLE_DVB = 0x00;
**/
#define pat_interval  50
enum YangTsStream{
	TS_H264,
	TS_H265,
	TS_AAC,
	TS_OPUS,
	TS_PRIVATE

};
struct YangTsPes{
	uint8_t *data;
	int32_t len;
	int32_t pos;
    uint64_t pts;
    uint64_t dts;
    uint64_t pcr;
    uint8_t stream_type;
    uint8_t stream_id;
    uint16_t pid;
};
class YangTsMuxer {
public:
	YangTsMuxer();
	virtual ~YangTsMuxer();
    void create_pat(YangTsBuffer *sb, uint16_t pmt_pid, uint8_t cc);
    void create_pmt(YangTsBuffer *sb,uint8_t cc);
    void create_ts(YangTsPes *frame,vector<YangTsBuffer>  *sb);
    void create_pes(YangTsPes *frame,uint8_t *p,int32_t plen,int32_t frametype,int64_t timestamp,YangTsStream streamType);
    void create_pcr(YangTsBuffer *sb);
    void create_null(YangTsBuffer *sb);
    void encode(uint8_t* p,int32_t plen,int32_t frametype,int64_t timestamp ,YangTsStream streamType,vector<YangTsBuffer> *sb);
    void encodeWithPmt(uint8_t* p,int32_t plen,int32_t frametype,int64_t timestamp ,YangTsStream streamType,vector<YangTsBuffer> *sb);
    //void encodePmt(vector<YangTsBuffer> *sb);
    void encodePmtWithoutData(vector<YangTsBuffer> *sb);
    std::map<uint8_t, int> m_stream_pid_map;
private:
    uint8_t get_cc(uint32_t with_pid);
    bool should_create_pat();
    int32_t m_pmt_pid;

      int32_t current_index;
private:
    std::map<uint32_t, uint8_t> _pid_cc_map;
};

#endif /* YANGTS_YANGTSMUXER_H_ */

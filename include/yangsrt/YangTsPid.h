/*
 * YangTsPid.h
 *
 *  Created on: 2020年10月16日
 *      Author: yang
 */

#ifndef SRC_YANGSRT_INCLUDE_YANGTSPID_H_
#define SRC_YANGSRT_INCLUDE_YANGTSPID_H_


/* mpegts stream type in ts pmt
Value    Description
0x00     ITU-T | ISO/IEC Reserved
0x01     ISO/IEC 11172-2 Video (mpeg video v1)
0x02     ITU-T Rec. H.262 | ISO/IEC 13818-2 Video(mpeg video v2)or ISO/IEC 11172-2 constrained parameter video stream
0x03     ISO/IEC 11172-3 Audio (MPEG 1 Audio codec Layer I, Layer II and Layer III audio specifications)
0x04     ISO/IEC 13818-3 Audio (BC Audio Codec)
0x05     ITU-T Rec. H.222.0 | ISO/IEC 13818-1 private_sections
0x06     ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing private data
0x07     ISO/IEC 13522 MHEG
0x08     ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC
0x09     ITU-T Rec. H.222.1
0x0A     ISO/IEC 13818-6 type A
0x0B     ISO/IEC 13818-6 type B
0x0C     ISO/IEC 13818-6 type C
0x0D     ISO/IEC 13818-6 type D
0x0E     ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary
0x0F     ISO/IEC 13818-7 Audio with ADTS transport syntax
0x10     ISO/IEC 14496-2 Visual
0x11     ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3/Amd.1
0x12     ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets
0x13     ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC 14496_sections
0x14     ISO/IEC 13818-6 Synchronized Download Protocol
0x15     Metadata carried in PES packets
0x16     Metadata carried in metadata_sections
0x17     Metadata carried in ISO/IEC 13818-6 Data Carousel
0x18     Metadata carried in ISO/IEC 13818-6 Object Carousel
0x19     Metadata carried in ISO/IEC 13818-6 Synchronized Download Protocol
0x1A     IPMP stream (defined in ISO/IEC 13818-11, MPEG-2 IPMP)
0x1B     AVC video stream as defined in ITU-T Rec. H.264 | ISO/IEC 14496-10 Video (h.264)
0x1C     ISO/IEC 14496-3 Audio, without using any additional transport syntax, such as DST, ALS and SLS
0x1D     ISO/IEC 14496-17 Text
0x1E     Auxiliary video stream as defined in ISO/IEC 23002-3 (AVS)
0x1F-0x7E ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved
0x7F     IPMP stream 0x80-0xFF User Private
*/
#define STREAM_TYPE_VIDEO_MPEG1     0x01
#define STREAM_TYPE_VIDEO_MPEG2     0x02
#define STREAM_TYPE_AUDIO_MPEG1     0x03
#define STREAM_TYPE_AUDIO_MPEG2     0x04
#define STREAM_TYPE_PRIVATE_SECTION 0x05
#define STREAM_TYPE_PRIVATE_DATA    0x06
#define STREAM_TYPE_AUDIO_AAC       0x0f
#define STREAM_TYPE_AUDIO_AAC_LATM  0x11

#define STREAM_TYPE_VIDEO_MPEG4     0x10
#define STREAM_TYPE_METADATA        0x15
#define STREAM_TYPE_VIDEO_H264      0x1b
#define STREAM_TYPE_VIDEO_HEVC      0x24
#define STREAM_TYPE_VIDEO_CAVS      0x42
#define STREAM_TYPE_VIDEO_VC1       0xea
#define STREAM_TYPE_VIDEO_DIRAC     0xd1

#define STREAM_TYPE_AUDIO_AC3       0x81
#define STREAM_TYPE_AUDIO_DTS       0x82
#define STREAM_TYPE_AUDIO_TRUEHD    0x83
#define STREAM_TYPE_AUDIO_EAC3      0x87

#define STREAM_TYPE_AUDIO_OPUS  0x12
#define PES_AUDIO_ID  0xc0
#define PES_VIDEO_ID  0xe0
//#define PES_VIDEO_ID  0xe1
#define Yang_H264_PID 225
#define Yang_H265_PID 226
#define Yang_AAC_PID 192
#define Yang_OPUS_PID 193
#define Yang_PRIVATE_PID 200

#endif /* SRC_YANGSRT_INCLUDE_YANGTSPID_H_ */

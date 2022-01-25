#ifndef __YangFlvWriter__
#define __YangFlvWriter__
#include "stdlib.h"
#include "stdio.h"
#include <stdint.h>
#include <yangutil/yangavinfotype.h>

//#include "flv_types.h"
//#include "as_objects.h"

#define		FLV_CODEC_NONE		-1
#define		FLV_CODEC_FLV1		2			// Sorenson H.263
#define		FLV_CODEC_FLV4		4			// On2 VP6
#define		FLV_CODEC_H264		7			// H.264

#define		FLV_CODEC_MP3		2			// MP3
#define		FLV_CODEC_AAC		10			// AAC


//typedef long __int64;
typedef uint8_t BYTE;
class YangFlvWriter {
public:

	int32_t video_codec;
	int32_t audio_codec;


	double video_fps;
	int32_t audio_channels;
	int32_t audio_samplerate;

	// time helpers
	bool is_first;
	int64_t time_first_ms;				// timestamp of the first packet
	int32_t time_last_ms;				// timestamp of the last packet
	int32_t duration_ms;		// calculated duration (for the onMetaData tag)
	int32_t video_raw_size;				// size of all video packets
	int32_t video_frames;				// total video frame count
	int32_t file_size;

	int32_t last_tag_size;				// helper for writing FLV file
	long metadatapos;				// byte position in the file



public:
	YangFlvWriter(char* fileName, YangAudioInfo *paudio,YangVideoInfo *pvideo);
	virtual ~YangFlvWriter();
	void setAudioAac();
	// stream config
	FILE *file;
	int32_t Reset();

	int32_t Start();
	int32_t Stop();
	void Close();
	//void ConfigAll();
	int32_t WriteVideoPacket(YangFrame* videoFrame);
	int32_t WriteAudioPacket(YangFrame* audioFrame);
	void WriteVideoInfo(uint8_t* buf1, int32_t buflen);
	double framerate, i_bitrate, i_level_idc;
	uint32_t  vtime,pre_vt;
	uint32_t  atime,pre_at;

	//int32_t tick, tickinterval, tickinterval1;
	//int32_t tick;

	double atime1;
	double perSt;
private:
	YangAudioInfo *m_audio;
	YangVideoInfo *m_video;
	BYTE prev[4];
	BYTE tag_hdr[20];
	// write the data
	int32_t m_Video_Bit_Count, m_Video_Width, m_Video_Height;

	short vcount;
	int32_t vtcou;
	int32_t WriteMetaData();

	static int32_t MakeAVCc(char* data, int32_t size, char *output_data,
			int32_t output_size);

	char * put_amf_string(char *c, const char *str);
	char * put_amf_double(char *c, double d);
	//char * put_byte(char *output, uint8_t nVal);

	//int32_t WriteExtradata(GUID type);
	//AM_MEDIA_TYPE *amt;
	//AM_MEDIA_TYPE *vmt;
	//int32_t ConfigStream(int32_t isVideo);
};

#endif

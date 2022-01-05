#include <yangrecord/YangFlvWrite.h>

#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <yangutil/sys/YangEndian.h>
#include "yangutil/yang_unistd.h"
#include "yangutil/sys/YangAmf.h"
#define REFERENCE_TIME int64_t
//#define _T(x) x
typedef uint8_t uint8;


//-----------------------------------------------------------------------------
//
//	FLVWriter class
//
//-----------------------------------------------------------------------------

YangFlvWriter::YangFlvWriter(char* fileName, YangAudioInfo *paudio,YangVideoInfo *pvideo)

{
	m_audio=paudio;
	m_video=pvideo;
	m_Video_Bit_Count = m_video->videoCaptureFormat==0?16:12;
	m_Video_Width = m_video->width;
	m_Video_Height = m_video->height;

	video_codec = FLV_CODEC_H264;
	audio_codec = m_audio->audioEncoderType==0?FLV_CODEC_AAC:FLV_CODEC_MP3;

	video_raw_size = 0;
	time_first_ms = 0;
	time_last_ms = 0;
	video_frames = 0;
	audio_channels = 0;
	audio_samplerate = 0;
	is_first = true;
	duration_ms = 0;
	video_fps = 0;
	file_size = 0;
	vcount = 0;
	vtime = 0;
	atime = 0;
	perSt = (m_audio->audioEncoderType==0?1024.0:1152.0) * 1000.0 / 44100.0;
	atime1 = 0;
	//vtime1 = 0;
	vtcou = 0;
	file = fopen(fileName,"wb");
	pre_vt=0;
	framerate=30;
	pre_at=0;
	metadatapos=0;
	i_bitrate=0;
	i_level_idc=0;
	last_tag_size=0;


}

YangFlvWriter::~YangFlvWriter() {
	m_audio=NULL;
		m_video=NULL;

	Reset();
}

int32_t YangFlvWriter::Reset() {

	video_raw_size = 0;
	video_frames = 0;
	audio_channels = 0;
	audio_samplerate = 0;
	last_tag_size = 0;
	video_fps = 0;
	file_size = 0;

	// no streams...
	video_codec = FLV_CODEC_H264;
	audio_codec = m_audio->audioEncoderType==0?FLV_CODEC_AAC:FLV_CODEC_MP3;

	time_first_ms = 0;
	time_last_ms = 0;
	duration_ms = 0;
	is_first = true;
	vtime = 0;
	atime = 0;

	return 0;
}

int32_t YangFlvWriter::Start() {

	// write the file header
	BYTE header[9] = { 'F', 'L', 'V',			// FLV file signature
			0x01,					// FLV file version = 1
			0,						// Flags - modified later
			0, 0, 0, 9				// size of the header
			};

	header[4] |= 0x01;
	//header[4] |= 0x04;
	//rewind(file);
	//io->Seek(0);
	file_size = 0;
	fwrite(header, 1, sizeof(header), file);

	metadatapos = ftell(file);
	WriteMetaData();

	file_size = ftell(file);
	metadatapos = file_size;
	return 0;
}

int32_t YangFlvWriter::Stop() {

	BYTE prev[4];
	//long ttt=vtime;
	prev[0] = (last_tag_size >> 24) & 0xff;
	prev[1] = (last_tag_size >> 16) & 0xff;
	prev[2] = (last_tag_size >> 8) & 0xff;
	prev[3] = (last_tag_size >> 0) & 0xff;
	fwrite(prev, 1, 4, file);
	uint8_t last[] = { 0x09, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x17, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x10 };
	last[4] = (vtime >> 16) & 0xff;		// TimeStamp			UI24
	last[5] = (vtime >> 8) & 0xff;
	last[6] = (vtime >> 0) & 0xff;
	last[7] = (vtime >> 24) & 0xff;
	file_size += (4 + sizeof(last));
	fwrite(last, 1, sizeof(last), file);
	//printf("\nvtime==%d,atime=%d,filesieze=%d\n", vtime, atime,file_size);
	rewind(file);
	fseek(file, 9, SEEK_SET);
	WriteMetaData();

	return 0;
}
void YangFlvWriter::Close() {
	Stop();
	fflush(file);
	fclose(file);
	file = NULL;
}
int32_t YangFlvWriter::MakeAVCc(char* data, int32_t size, char *output_data,
		int32_t output_size) {
	if (!data || size <= 0)
		return -1;
	int32_t ps_size = (data[0] << 8) | (data[1]);
	int32_t ss_size = (data[ps_size + 2] << 8) | (data[ps_size + 3]);
	int32_t buf_size = 6 + ps_size + 2 + 1 + ss_size + 2;

	if (buf_size > output_size)
		return -1;

	char* temp = data;
	char* output_temp = output_data;

	output_temp[0] = 1;
	output_temp[1] = temp[3];
	output_temp[2] = temp[4];
	output_temp[3] = temp[5];
	output_temp[4] = 0xff;
	output_temp[5] = 0xe1;
	output_temp += 6;

	memcpy(output_temp, temp, ps_size + 2);
	output_temp += ps_size + 2;
	temp += ps_size + 2;

	output_temp[0] = 1;
	output_temp += 1;

	memcpy(output_temp, temp, ss_size + 2);

	return buf_size;
}

void YangFlvWriter::WriteVideoInfo(uint8_t* buf1, int32_t buflen) {

	/**BYTE prev[4];
	BYTE tag_hdr[16];
	int32_t tag_data_size(0);
	int32_t towrite(0);
	BYTE *data(NULL);
	int32_t size(0);**/

	memset(tag_hdr, 0, sizeof(tag_hdr));
	//memset(prev,0,4);
	prev[0] = (last_tag_size >> 24) & 0xff;
	prev[1] = (last_tag_size >> 16) & 0xff;
	prev[2] = (last_tag_size >> 8) & 0xff;
	prev[3] = (last_tag_size >> 0) & 0xff;
	fwrite(prev, 1, 4, file);

	//int32_t extradata_size = MakeAVCc(video_extradata.data, video_extradata.size, extradata, sizeof(extradata));
	//tag_data_size = 5 + extradata_size;
	tag_hdr[0] = 0x09;
	tag_hdr[11] = 0x17;
	//towrite = 16;
	//data = (BYTE*)extradata;
	//size = extradata_size;

	tag_hdr[1] = (buflen >> 16) & 0xff;
	tag_hdr[2] = (buflen >> 8) & 0xff;
	tag_hdr[3] = (buflen >> 0) & 0xff;

	//last_tag_size = tag_data_size + 11;
	fwrite(tag_hdr, 1, 11, file);
	fwrite(buf1, 1, buflen, file);

}

int32_t YangFlvWriter::WriteAudioPacket(YangFrame* audioFrame) {
	if (!file)
		return 0;
	//printf("%d,",p_len);
	//BYTE tag_hdr[20];
	memset(tag_hdr, 0, sizeof(tag_hdr));
	tag_hdr[0] = 0x08;
	int p_len=audioFrame->nb;
	int32_t tag_data_size = p_len;
	tag_data_size += 1;
	tag_hdr[1] = ((p_len + 1) >> 16) & 0xff;
	tag_hdr[2] = ((p_len + 1) >> 8) & 0xff;
	tag_hdr[3] = ((p_len + 1) >> 0) & 0xff;

	//__int64 timestamp_ms = 0;
	pre_at = atime;


	vtcou++;
	atime1 += perSt;
	atime = (unsigned int) atime1;
	tag_hdr[4] = (pre_at >> 16) & 0xff;		// TimeStamp			UI24
	tag_hdr[5] = (pre_at >> 8) & 0xff;
	tag_hdr[6] = (pre_at >> 0) & 0xff;
	tag_hdr[7] = (pre_at >> 24) & 0xff;		// TimestampExtended	UI8

	// keep track of the last timestamp
	time_last_ms = pre_at;
	duration_ms = time_last_ms;	// for now we consider the last timestamp duration

	// StreamID = always 0
	tag_hdr[8] = 0;
	tag_hdr[9] = 0;
	tag_hdr[10] = 0;

	/*
	 Now write the TAG
	 */

	// 1. previous tag size
	//BYTE prev[4];
	prev[0] = (last_tag_size >> 24) & 0xff;
	prev[1] = (last_tag_size >> 16) & 0xff;
	prev[2] = (last_tag_size >> 8) & 0xff;
	prev[3] = (last_tag_size >> 0) & 0xff;

	fwrite(prev, 1, 4, file);
	tag_hdr[11] = 0x2f;
	fwrite(tag_hdr, 1, 12, file);
	fwrite(audioFrame->payload, 1, p_len, file);
	last_tag_size = p_len + 12;
	file_size = ftell(file);					//io->GetPosition();
	return 1;

}
int32_t YangFlvWriter::WriteVideoPacket(YangFrame* videoFrame) {
	if (!file)
		return 0;


	memset(tag_hdr, 0, sizeof(tag_hdr));
	// we support only two streams

	tag_hdr[0] = 0x09;
	//printf("-a%d-", vtime);
	// tag size
	int32_t len=videoFrame->nb;
	int32_t tag_data_size = len;

	// VIDEO DATA follows after the tag
	tag_data_size += 5;

	tag_hdr[1] = ((len + 9) >> 16) & 0xff;
	tag_hdr[2] = ((len + 9) >> 8) & 0xff;
	tag_hdr[3] = ((len + 9) >> 0) & 0xff;

	time_first_ms = 0;			// we will offset all timestamps by this value
	pre_vt=vtime;
	vtime = videoFrame->timestamp;
	//printf("%d,",p_timestamp);
	//vcount++;

	tag_hdr[4] = (pre_vt >> 16) & 0xff;		// TimeStamp			UI24
	tag_hdr[5] = (pre_vt >> 8) & 0xff;
	tag_hdr[6] = (pre_vt >> 0) & 0xff;
	tag_hdr[7] = (pre_vt >> 24) & 0xff;		// TimestampExtended	UI8

	// keep track of the last timestamp
	time_last_ms = pre_vt;
	duration_ms = time_last_ms;	// for now we consider the last timestamp duration

	// StreamID = always 0
	tag_hdr[8] = 0;
	tag_hdr[9] = 0;
	tag_hdr[10] = 0;

	/*
	 Now write the TAG
	 */

	// 1. previous tag size

	prev[0] = (last_tag_size >> 24) & 0xff;
	prev[1] = (last_tag_size >> 16) & 0xff;
	prev[2] = (last_tag_size >> 8) & 0xff;
	prev[3] = (last_tag_size >> 0) & 0xff;

	fwrite(prev, 1, 4, file);

	if (videoFrame->frametype == 1)
		tag_hdr[11] = 0x17;
	else
		tag_hdr[11] = 0x27;

	tag_hdr[12] = 0x01;
	//int32_t diff=33;
	// tag_hdr[15]=0x42;
	tag_hdr[19] = len & 0xff;
	tag_hdr[18] = len >> 8;
	tag_hdr[17] = len >> 16;
	tag_hdr[16] = len >> 24;

	fwrite(tag_hdr, 1, 20, file);
	fwrite(videoFrame->payload, 1, len, file);

	video_raw_size += len;     //packet->size;
	video_frames += 1;

	last_tag_size = len + 20;
	file_size = ftell(file);		//io->GetPosition();
	return 0;
}

int32_t YangFlvWriter::WriteMetaData() {

	/*
	 We assemble some basic onMetaData structure.
	 */
	//Flash::AS_String name;
	//Flash::AS_ECMA_Array vals;

	//name.value = _T("onMetaData");

	/*
	 We create the following metadata
	 */
	char meta[1024];
	 char * szTmp=meta;

	// char * szTmp=(char *)temp;

	 // szTmp=yang_put_byte(szTmp, AMF_STRING );
	 // szTmp=put_amf_string(szTmp, "@setDataFrame" );
	 szTmp=yang_put_byte(szTmp, AMF_STRING );
	 szTmp=put_amf_string(szTmp, "onMetaData" );
	 szTmp=yang_put_byte(szTmp, AMF_OBJECT );

	 szTmp=put_amf_string( szTmp, "metadatacreator" );
	 szTmp=yang_put_byte(szTmp, AMF_STRING );
	 szTmp=put_amf_string( szTmp, "FLV Mux" );

	 szTmp=put_amf_string( szTmp, "duration" );
	 szTmp=put_amf_double( szTmp, vtime/1000 );

	 szTmp=put_amf_string( szTmp, "filesize" );
	 szTmp=put_amf_double( szTmp, file_size );

	 szTmp=put_amf_string( szTmp, "lasttimestamp" );
	 szTmp=put_amf_double( szTmp, time_last_ms/1000 );


	 szTmp=put_amf_string( szTmp, "hasVideo" );
		 szTmp=put_amf_double( szTmp, 1 );



	 //szTmp=put_amf_string( szTmp, "Custom" );

	 szTmp=put_amf_string( szTmp, "width" );
	 szTmp=put_amf_double( szTmp, m_Video_Width );

	 szTmp=put_amf_string( szTmp, "height" );
	 szTmp=put_amf_double( szTmp, m_Video_Height );

	 szTmp=put_amf_string( szTmp, "framerate" );
	 szTmp=put_amf_double( szTmp, m_video->frame);

	 szTmp=put_amf_string( szTmp, "videocodecid" );
	 szTmp=yang_put_byte(szTmp, AMF_STRING );
	 szTmp=put_amf_string( szTmp, "avc1" );

	 szTmp=put_amf_string( szTmp, "videodatarate" );
	 szTmp=put_amf_double( szTmp, i_bitrate );

	 szTmp=put_amf_string( szTmp, "avclevel" );
	 szTmp=put_amf_double( szTmp, i_level_idc );

	 szTmp=put_amf_string( szTmp, "avcprofile" );
	 szTmp=put_amf_double( szTmp, 0x42 );

	 szTmp=put_amf_string( szTmp, "videokeyframe_frequency" );
	 szTmp=put_amf_double( szTmp, 3 );
	// szTmp=put_amf_string( szTmp, "" );

		szTmp=put_amf_string( szTmp, "hasAudio" );
	    szTmp=put_amf_double( szTmp, 1);

	    szTmp=put_amf_string( szTmp, "audiocodecid" );
		szTmp=put_amf_double( szTmp, 2 );


		 szTmp=put_amf_string( szTmp, "audiosamplesize" );
	    szTmp=put_amf_double( szTmp, 16 );

	    szTmp=put_amf_string( szTmp, "stereo" );
	    szTmp=put_amf_double( szTmp, 1 );

	    szTmp=put_amf_string( szTmp, "audiosamplerate" );
	    szTmp=put_amf_double( szTmp, 44100 );

	    szTmp=put_amf_string( szTmp, "" );
	 szTmp=yang_put_byte( szTmp, AMF_OBJECT_END );

	int32_t total_size=szTmp-meta;
	BYTE tag_hdr[] = { 0, 0, 0, 0,							// previous tag size
			0x12,						// Type			UI8 = Script Data Tag,
			(total_size >> 16) & 0xff,			// DataSize     UI24
			(total_size >> 8) & 0xff, (total_size >> 0) & 0xff, 0, 0, 0, 0,	// TimeStamp	UI24   + TimestampExtended UI8
			0, 0, 0							// StreamID		UI24  (always 0)
			};

	fwrite(tag_hdr, 1, sizeof(tag_hdr), file);
	fwrite(meta, 1, total_size, file);
	last_tag_size = (sizeof(tag_hdr) + total_size - 4);	// the first 4 bytes don't count
	szTmp=NULL;


	return 0;
}


char * YangFlvWriter::put_amf_string( char *c, const char *str )
{
    uint16_t len = strlen( str );
    c=yang_put_be16( c, len );
    memcpy(c,str,len);
    return c+len;
}
char * YangFlvWriter::put_amf_double( char *c, double d )
{
    *c++ = 0;  /* type: Number */
    {
        uint8_t *ci, *co;
        ci = (uint8_t *)&d;
        co = (uint8_t *)c;
        co[0] = ci[7];
        co[1] = ci[6];
        co[2] = ci[5];
        co[3] = ci[4];
        co[4] = ci[3];
        co[5] = ci[2];
        co[6] = ci[1];
        co[7] = ci[0];
    }
    return c+8;
}


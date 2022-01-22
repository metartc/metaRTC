#ifndef __YangAudioMp3Encoder__
#define __YangAudioMp3Encoder__
#include <yangavutil/audio/YangMakeWave.h>
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
extern "C"{
#include "lame.h"
}
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangencoder/YangAudioEncoder.h"
#include "yangutil/sys/YangLoadLib.h"
#ifndef _WIN32
typedef int32_t DWORD ;
#endif
class YangAudioEncoderMp3:public YangAudioEncoder
{
public:
	YangAudioEncoderMp3();
	~YangAudioEncoderMp3(void);
	void init(YangAudioInfo *pap);
	int32_t encoder(YangFrame* pframe,YangEncoderCallback* pcallback);


private:
	uint8_t *temp;//[1153*2+44100*2];
	uint8_t *pMP3Buffer;
	short*		pWAVBuffer;
	lame_global_flags *gfp;
    int	m_Lame_Sample_Number;
    int32_t m_bufLen;//m_Audio_Frame_Size,;
	DWORD		dwSamples;
	DWORD dwRead;
	DWORD dwWrite;
	DWORD dwDone;
	DWORD dwFileSize;
	int32_t preSize;

private:
	YangLoadLib m_lib;
	void closeMp3();
	int	dwMP3Buffer	;
	int32_t ret;
	void loadLib();
	void unloadLib();
    lame_global_flags *  (*yang_lame_init)(void);
    int32_t  (*yang_lame_init_params)(lame_global_flags *);
    int32_t  (*yang_lame_set_preset)( lame_global_flags*  gfp, int32_t );
    int32_t  (*yang_lame_set_in_samplerate)(lame_global_flags *, int);
    int32_t  (*yang_lame_set_VBR)(lame_global_flags *, vbr_mode);
    int32_t  (*yang_lame_set_mode)(lame_global_flags *, MPEG_mode);
    int32_t  (*yang_lame_set_num_channels)(lame_global_flags *, int);
    int32_t  (*yang_lame_set_brate)(lame_global_flags *, int);
    int32_t  (*yang_lame_set_strict_ISO)(lame_global_flags *, int);
    int32_t  (*yang_lame_set_original)(lame_global_flags *, int);
    int32_t  (*yang_lame_set_error_protection)(lame_global_flags *, int);
    int32_t  (*yang_lame_set_extension)(lame_global_flags *, int);
    int32_t  (*yang_lame_set_disable_reservoir)(lame_global_flags *, int);
    int32_t  (*yang_lame_set_bWriteVbrTag)(lame_global_flags *, int);
    int32_t  (*yang_lame_encode_buffer_interleaved)(lame_global_flags*  gfp,
	        int16_t            pcm[],int32_t   num_samples, uint8_t*      mp3buf,
	        int32_t                 mp3buf_size );
    int32_t   (*yang_lame_close) (lame_global_flags *);

};
#endif


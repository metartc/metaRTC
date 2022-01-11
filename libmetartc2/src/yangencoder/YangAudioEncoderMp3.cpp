#include "YangAudioEncoderMp3.h"

#include "yangutil/yang_unistd.h"
#include "stdio.h"

void YangAudioEncoderMp3::loadLib() {

    yang_lame_init = (lame_global_flags*  (*)(void))m_lib.loadFunction("lame_init");
    yang_lame_init_params=(int32_t  (*)(lame_global_flags *))m_lib.loadFunction("lame_init_params");
    yang_lame_set_preset=(int32_t  (*)( lame_global_flags* gfp, int32_t ))m_lib.loadFunction("lame_set_preset");
    yang_lame_set_in_samplerate=(int32_t  (*)(lame_global_flags *, int))m_lib.loadFunction("lame_set_in_samplerate");
    yang_lame_set_VBR=(int32_t  (*)(lame_global_flags *, vbr_mode))m_lib.loadFunction("lame_set_VBR");
    yang_lame_set_mode=(int32_t  (*)(lame_global_flags *, MPEG_mode))m_lib.loadFunction("lame_set_mode");
    yang_lame_set_num_channels=(int32_t  (*)(lame_global_flags *, int))m_lib.loadFunction("lame_set_num_channels");
    yang_lame_set_brate=(int32_t  (*)(lame_global_flags *, int))m_lib.loadFunction("lame_set_brate");
    yang_lame_set_strict_ISO=(int32_t  (*)(lame_global_flags *, int))m_lib.loadFunction("lame_set_strict_ISO");
    yang_lame_set_original=(int32_t  (*)(lame_global_flags *, int))m_lib.loadFunction("lame_set_original");
    yang_lame_set_error_protection=(int32_t  (*)(lame_global_flags *, int))m_lib.loadFunction("lame_set_error_protection");
    yang_lame_set_extension=(int32_t  (*)(lame_global_flags *, int))m_lib.loadFunction("lame_set_extension");
    yang_lame_set_disable_reservoir=(int32_t  (*)(lame_global_flags *, int))m_lib.loadFunction("lame_set_disable_reservoir");
    yang_lame_set_bWriteVbrTag=(int32_t  (*)(lame_global_flags *, int))m_lib.loadFunction("lame_set_bWriteVbrTag");
    yang_lame_encode_buffer_interleaved=(int32_t  (*)(lame_global_flags* gfp,
			int16_t  pcm[],int32_t num_samples, uint8_t* mp3buf,
			int32_t mp3buf_size ))m_lib.loadFunction("lame_encode_buffer_interleaved");
    yang_lame_close=(int32_t  (*) (lame_global_flags *))m_lib.loadFunction("lame_close");
}

void YangAudioEncoderMp3::unloadLib() {
	yang_lame_init = NULL;
	yang_lame_init_params = NULL;
	yang_lame_set_preset = NULL;
	yang_lame_set_in_samplerate = NULL;
	yang_lame_set_VBR = NULL;
	yang_lame_set_mode = NULL;
	yang_lame_set_num_channels = NULL;
	yang_lame_set_brate = NULL;
	yang_lame_set_strict_ISO = NULL;
	yang_lame_set_original = NULL;
	yang_lame_set_error_protection = NULL;
	yang_lame_set_extension = NULL;
	yang_lame_set_disable_reservoir = NULL;
	yang_lame_set_bWriteVbrTag = NULL;
	yang_lame_encode_buffer_interleaved = NULL;
	yang_lame_close = NULL;
}

YangAudioEncoderMp3::YangAudioEncoderMp3() {
	m_Lame_Sample_Number = 1152;
	ret = 0;
	preSize = 0;
	dwSamples = 0;
	dwRead = 0;
	dwWrite = 0;
	dwDone = 0;
	dwFileSize = 0;
	m_bufLen = 0;

	dwMP3Buffer = 0;
	pMP3Buffer = NULL;
	pWAVBuffer = NULL;
	gfp = NULL;
	temp = NULL;
	//isConvert = 0;
	unloadLib();

}

YangAudioEncoderMp3::~YangAudioEncoderMp3(void) {
	closeMp3();
	yang_deleteA(temp);
	yang_deleteA(pWAVBuffer);
	yang_deleteA(pMP3Buffer);
	unloadLib();
	m_lib.unloadObject();

}

void YangAudioEncoderMp3::init(YangAudioInfo *pap) {
	if (m_isInit == 1)
		return;
	m_lib.loadObject("libmp3lame");
	loadLib();
	setAudioPara(pap);
	dwSamples = 0;
	dwMP3Buffer = 0;
	pMP3Buffer = NULL;
	pWAVBuffer = NULL;

	gfp = NULL;
	gfp = yang_lame_init();
	yang_lame_set_in_samplerate(gfp, m_audioInfo.sample);

	yang_lame_set_preset(gfp, m_audioInfo.bitrate);
	//lame_set_preset( gfp, R3MIX);
	yang_lame_set_VBR(gfp, vbr_off);
	yang_lame_set_mode(gfp, STEREO);	//JOINT_STEREO STEREO
	yang_lame_set_num_channels(gfp, 2);
	yang_lame_set_brate(gfp, m_audioInfo.bitrate);
	//lame_set_quality(gfp,);
	yang_lame_set_strict_ISO(gfp, 1);
	yang_lame_set_original(gfp, 1);
	yang_lame_set_error_protection(gfp, 0);
	yang_lame_set_extension(gfp, 0);
	yang_lame_set_disable_reservoir(gfp, 1);
	yang_lame_set_bWriteVbrTag(gfp, 1);
	int32_t ret_code = yang_lame_init_params(gfp);
	if (ret_code < 0) {
		printf("lame_init_params failure returned %d\n", ret_code);
	} else {
		//printf("lame_init_params success returned %d\n", ret_code);
	}

	dwSamples = 1152 * 2;
	dwMP3Buffer = (DWORD) (1.25 * (dwSamples / 2) + 7200);

	pMP3Buffer = new uint8_t[dwMP3Buffer];
	pWAVBuffer = new short[dwSamples * 8];

	temp = new uint8_t[1152 * 8];
	//dwRead=0;
	dwWrite = 0;
	dwDone = 0;
	dwFileSize = 0;
	m_isInit = 1;
	dwRead = m_Lame_Sample_Number * 4;

}


int32_t YangAudioEncoderMp3::encoder(YangFrame* pframe,YangEncoderCallback *pcallback) {
	memcpy(temp + m_bufLen, pframe->payload, pframe->nb);

	m_bufLen += pframe->nb;
	if (m_bufLen < dwRead)
		return 1;

	memcpy(pWAVBuffer, temp, dwRead);
	dwWrite = yang_lame_encode_buffer_interleaved(gfp, pWAVBuffer, 1152,
			pMP3Buffer, 0);

	if (dwWrite > 0) {
		pframe->payload=pMP3Buffer;
		pframe->nb=dwWrite;
		pcallback->onAudioData(pframe);
		return Yang_Ok;

	}
	m_bufLen -= dwRead;

	if (m_bufLen < dwRead && m_bufLen > 0) {
		memcpy(temp, temp + dwRead, m_bufLen);
	}
	return Yang_Ok;
}

void YangAudioEncoderMp3::closeMp3() {
	if (gfp != NULL)
		yang_lame_close(gfp);
	gfp = NULL;
	ret = 2;

}


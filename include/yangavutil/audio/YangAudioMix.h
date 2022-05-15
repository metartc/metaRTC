//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGUTIL_AUDIO_YANGAUDIOMIX_H_
#define YANGUTIL_AUDIO_YANGAUDIOMIX_H_
#include <stdint.h>
#define SIZE_AUDIO_FRAME 4096
#ifdef __cplusplus
extern "C" {
#endif

void yang_mixaudio_mix1(short *dst, short *src, int32_t len1, int32_t volume);
void yang_mixaudio_mix2(uint8_t *dst, uint8_t *src,int32_t len,int32_t pvolume);
void yang_mixaudio_mix4(uint8_t *dst, uint8_t *src, int32_t len,int32_t pvolume) ;
void yang_mixaudio_mix3(uint8_t *dst, uint8_t *src, int32_t len,int32_t pvolume);
void yang_mixaudio_mix5(uint8_t *dst, uint8_t *src, int32_t len1,int32_t pvolume);

#ifdef __cplusplus
}
#endif
#endif /* YANGUTIL_AUDIO_YANGAUDIOMIX_H_ */

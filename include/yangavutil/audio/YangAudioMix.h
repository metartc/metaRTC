/*
 * YangAudioMix.h
 *
 *  Created on: 2020年9月25日
 *      Author: yang
 */

#ifndef YANGUTIL_AUDIO_YANGAUDIOMIX_H_
#define YANGUTIL_AUDIO_YANGAUDIOMIX_H_
#include <stdint.h>
#define SIZE_AUDIO_FRAME 4096

class YangAudioMix {
public:
	YangAudioMix();
	virtual ~YangAudioMix();
	void yangMix4(uint8_t *dst, uint8_t *src, int32_t len,int32_t pvolume);
	void yangMix3(uint8_t *dst, uint8_t *src, int32_t len,int32_t pvolume);
	//void yangMix(uint8_t *sourseFile[SIZE_AUDIO_FRAME], int32_t number,	uint8_t *objectFile);
	//void yangMixs1(uint8_t *dst, uint8_t *src, int32_t len, int32_t volume);
	void yangMix1(uint8_t *dst, uint8_t *src, int32_t len, int32_t volume);
	void yangMix1(short *dst, short *src, int32_t len, int32_t volume);
	void yangMix2(uint8_t *pin, uint8_t *pout,int32_t len,int32_t pvolume);
private:
	int32_t  m_yangAudioMAX;
	int32_t  m_yangAudioMIN ;
};

#endif /* YANGUTIL_AUDIO_YANGAUDIOMIX_H_ */

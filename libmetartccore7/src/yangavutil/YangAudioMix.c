//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangavutil/audio/YangAudioMix.h>
#include <math.h>



#define m_yangAudioMAX  (int32_t) ((1 << (16 - 1)) - 1)
#define	m_yangAudioMIN   (int32_t)(-(1 << (16 - 1)))

#define Yang_MIX_MAXVOLUME 128
#define ADJUST_VOLUME(s, v) (s = (s*v)/Yang_MIX_MAXVOLUME)

void yang_mixaudio_mix1(short *dst, short *src, int32_t len1, int32_t volume){
	short src1, src2;
	int32_t output;

	int32_t len = len1/2;
	for(int32_t i=0;i<len;i++) {
		src1=src[i];
		src2=dst[i];
		output = src1 + src2;
		if (output > m_yangAudioMAX) {
			output = m_yangAudioMAX;
		} else if (output < m_yangAudioMIN) {
			output = m_yangAudioMIN;
		}
		dst[i]=output;
	}
}
void yang_mixaudio_mix2(uint8_t *dst, uint8_t *src,int32_t len,int32_t pvolume) {
	double f = 1;
	short src1, src2;
	int32_t output;

	len /= 2;
	while (len--) {
		src1 = ((src[1]) << 8 | src[0]);
		// ADJUST_VOLUME(src1, volume);
		src2 = ((dst[1]) << 8 | dst[0]);
		src += 2;
		output = (src1 + src2)*f;
		if (output > m_yangAudioMAX) {
			output = m_yangAudioMAX;
			f = (double) m_yangAudioMAX / (double) (output);
		} else if (output < m_yangAudioMIN) {
			output = m_yangAudioMIN;
			f = (double) m_yangAudioMIN / (double) (output);
		}
		if (f < 1) {
				f += ((double) 1 - f) / (double) 32;
			}
		dst[0] = output & 0xFF;
		output >>= 8;
		dst[1] = output & 0xFF;
		dst += 2;
	}
}


void yang_mixaudio_mix3(uint8_t *dst, uint8_t *src, int32_t len,int32_t pvolume) {
	short src1, src2;
	int32_t output;

	len /= 2;
	while (len--) {
		src1 = ((src[1]) << 8 | src[0]);
		// ADJUST_VOLUME(src1, pvolume);
		src2 = ((dst[1]) << 8 | dst[0]);
		src += 2;
		output = (src1+src2)/2;//FloatS16ToS16(src1 + src2);
		if (output > m_yangAudioMAX) {

			output = m_yangAudioMAX;
		} else if (output < m_yangAudioMIN) {

			output = m_yangAudioMIN;
		}
		dst[0] = output & 0xFF;
		output >>= 8;
		dst[1] = output & 0xFF;
		dst += 2;
	}
}


void yang_mixaudio_mix4(uint8_t *dst, uint8_t *src, int32_t len,int32_t pvolume) {
	short src1, src2;
	int32_t output;

	len /= 2;
	while (len--) {
		src1 = ((src[1]) << 8 | src[0]);

		src2 = ((dst[1]) << 8 | dst[0]);
		src += 2;
		if( src1 < 0 && src2 < 0)
			output = src1+src2 - (src1 * src2 / -(pow(2,16-1)-1));
		else
			output = src1+src2 - (src1 * src2 / (pow(2,16-1)-1));
		if (output > m_yangAudioMAX) {

			output = m_yangAudioMAX;
		} else if (output < m_yangAudioMIN) {

			output = m_yangAudioMIN;
		}
		dst[0] = output & 0xFF;
		output >>= 8;
		dst[1] = output & 0xFF;
		dst += 2;
	}
}

void yang_mixaudio_mix5(uint8_t *dst, uint8_t *src, int32_t len1,int32_t pvolume) {
	short src1, src2;
	int32_t output;

	int32_t len = len1/2;
	while (len--) {
		src1 = ((src[1]) << 8 | src[0]);
		// ADJUST_VOLUME(src1, pvolume);
		src2 = ((dst[1]) << 8 | dst[0]);
		src += 2;
		output = src1 + src2;
		if (output > m_yangAudioMAX) {
			output = m_yangAudioMAX;
		} else if (output < m_yangAudioMIN) {
			output = m_yangAudioMIN;
		}
		dst[0] = output & 0xFF;
		output >>= 8;
		dst[1] = output & 0xFF;
		dst += 2;
	}
}

//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef  _YangMakeWave_H_
#define  _YangMakeWave_H_

#include <yangutil/yangtype.h>

typedef struct {
	char fileID[4];
	int32_t fileleth;
	char wavTag[4];
	char FmtHdrID[4];
	int32_t FmtHdrLeth;
	int16_t FormatTag;
	int16_t Channels;
	int32_t SamplesPerSec;
	int32_t AvgBytesPerSec;
	int16_t BlockAlign;
	int16_t BitsPerSample;
	char DataHdrID[4];
	int32_t DataHdrLeth;
} WaveHdr;
class YangMakeWave {
public:
	YangMakeWave();
	FILE *waveFile;
	void init();
	void start(int32_t pisMono, int32_t sample, char *filename);
	void write(uint8_t *data, int32_t len);
	void stop();
	void writeHeader(int32_t isMono, int32_t sample, FILE *WavFile,
			int32_t len);

	int32_t m_pcmLength;
	int32_t m_channel;
	int32_t m_sample;
};
#endif

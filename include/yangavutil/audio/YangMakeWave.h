//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef  _YangMakeWave_H_
#define  _YangMakeWave_H_

#include <stdint.h>
#include "stdio.h"

typedef  short Int16;
typedef  int32_t Int32;
typedef struct  {
	char fileID[4];
	Int32 fileleth;
	char wavTag[4];
	char FmtHdrID[4];
	Int32  FmtHdrLeth;
	Int16 FormatTag;
	Int16 Channels;
	Int32 SamplesPerSec;
	Int32 AvgBytesPerSec;
	Int16 BlockAlign;
	Int16 BitsPerSample;
	char DataHdrID[4];
	Int32  DataHdrLeth;
} WaveHdr;
class YangMakeWave{
public:
	YangMakeWave();
	FILE *waveFile;
	void init();
	void start(int32_t pisMono,char * filename);
	void write(uint8_t *data,int32_t len);
	void stop();
void writeHeader(int32_t isMono,FILE *WavFile,unsigned long len);
int32_t isMp3(char* p);

int32_t PCMSize;
int32_t m_isMono;
};
#endif

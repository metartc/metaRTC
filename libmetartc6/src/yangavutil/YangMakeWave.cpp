//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangavutil/audio/YangMakeWave.h>
YangMakeWave::YangMakeWave() {
	waveFile = NULL;
	m_pcmLength = 0;
	m_channel = 2;
	m_sample = 48000;
}

void YangMakeWave::write(uint8_t *data, int32_t len) {
	fwrite(data, 1, len, waveFile);
	fflush(waveFile);
	m_pcmLength += len;

}
void YangMakeWave::start(int32_t sample,int32_t channel, char *filename) {
	m_channel = channel;
	m_sample = sample;
	waveFile = fopen(filename, "wb");
	writeHeader( sample,channel, waveFile, 0);
	m_pcmLength = 0;
}
void YangMakeWave::stop() {
	writeHeader(m_sample, m_channel,waveFile, m_pcmLength);
	fclose(waveFile);
	waveFile = NULL;
}
void YangMakeWave::writeHeader(int32_t sample,int32_t channel, FILE *WavFile,
		int32_t len) {

	WaveHdr WaveHeader;

	WaveHeader.fileID[0] = 'R';
	WaveHeader.fileID[1] = 'I';
	WaveHeader.fileID[2] = 'F';
	WaveHeader.fileID[3] = 'F';
	WaveHeader.fileleth = 0;
	WaveHeader.wavTag[0] = 'W';
	WaveHeader.wavTag[1] = 'A';
	WaveHeader.wavTag[2] = 'V';
	WaveHeader.wavTag[3] = 'E';

	WaveHeader.FmtHdrID[0] = 'f';
	WaveHeader.FmtHdrID[1] = 'm';
	WaveHeader.FmtHdrID[2] = 't';
	WaveHeader.FmtHdrID[3] = ' ';

	WaveHeader.FmtHdrLeth = 0;

//	ChunkHdr FmtHdr = {"fmt ",};
	WaveHeader.DataHdrID[0] = 'd';
	WaveHeader.DataHdrID[1] = 'a';
	WaveHeader.DataHdrID[2] = 't';
	WaveHeader.DataHdrID[3] = 'a';
	WaveHeader.DataHdrLeth = 0;
	rewind(WavFile);

	WaveHeader.fileleth = len + 32;
	WaveHeader.FmtHdrLeth = 16;
	WaveHeader.BitsPerSample = 16;
	WaveHeader.Channels = 2;
	WaveHeader.FormatTag = 0x0001;

	WaveHeader.SamplesPerSec = sample;
	WaveHeader.AvgBytesPerSec = channel * 2 * sample;

	WaveHeader.BlockAlign = 4;

	WaveHeader.DataHdrLeth = len;

	fwrite(&WaveHeader, sizeof(WaveHdr), 1, WavFile);
	fflush(WavFile);

}


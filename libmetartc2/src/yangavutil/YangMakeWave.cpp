#include <yangavutil/audio/YangMakeWave.h>
YangMakeWave::YangMakeWave(){
	waveFile=NULL;
	PCMSize=0;
	 m_isMono=0;
}
/**
int32_t YangMakeWave::isMp3(char *p){
	//char p[20];
	//printf("a==%c,%c,%c",p[0],p[2],p[22]);
	if(p[0]=='M'&&p[2]=='P'&&p[22]=='3') {
		printf("****************mp3");
		return 1
			;}
	return 0;
}**/
void YangMakeWave::write(uint8_t * data,int32_t len){
fwrite(data,1,len,waveFile);
fflush(waveFile);
PCMSize+=len;

}
void YangMakeWave::start(int32_t pisMono,char * filename){
	m_isMono=pisMono;
	waveFile=fopen(filename,"wb");
	writeHeader(m_isMono,waveFile,0);
	PCMSize=0;
}
void YangMakeWave::stop(){
	writeHeader(m_isMono,waveFile,PCMSize);
	fclose(waveFile);
	waveFile=NULL;
}
void YangMakeWave::writeHeader(int32_t isMono,FILE *WavFile,unsigned long len){
	
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
	if(isMono){
		WaveHeader.SamplesPerSec = 16000;
		WaveHeader.AvgBytesPerSec = 2*16000;
	}else{
		WaveHeader.SamplesPerSec = 44100;
		WaveHeader.AvgBytesPerSec = 4*44100;
	}

	WaveHeader.BlockAlign = 4;

    WaveHeader.DataHdrLeth = len;
	
	fwrite(&WaveHeader,sizeof(WaveHdr),1,WavFile);
	fflush( WavFile );

}


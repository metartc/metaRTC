#include <yangrecord/YangMp4File.h>
#include <yangutil/sys/YangLog.h>
void YangMp4File::loadLib() {
	yang_MP4TagsAlloc = (const MP4Tags* (*)(void)) m_lib.loadFunction("MP4TagsAlloc");
    yang_MP4TagsFree=(void (*)( const MP4Tags* tags )) m_lib.loadFunction("MP4TagsFree");
	yang_MP4Create =(MP4FileHandle (*)(const char *fileName, uint32_t flags)) m_lib.loadFunction("MP4Create");
	yang_MP4TagsFetch =(bool (*)(const MP4Tags *tags, MP4FileHandle hFile)) m_lib.loadFunction("MP4TagsFetch");
	yang_MP4TagsSetSortArtist =	(bool (*)(const MP4Tags*, const char*)) m_lib.loadFunction("MP4TagsSetSortArtist");
	yang_MP4TagsStore =(bool (*)(const MP4Tags *tags, MP4FileHandle hFile)) m_lib.loadFunction("MP4TagsStore");
	yang_MP4SetTimeScale =(bool (*)(MP4FileHandle hFile, uint32_t value)) m_lib.loadFunction("MP4SetTimeScale");
	yang_MP4SetVideoProfileLevel =(void (*)(MP4FileHandle hFile, uint8_t value))m_lib.loadFunction("MP4SetVideoProfileLevel");
	yang_MP4SetAudioProfileLevel =(void (*)(MP4FileHandle hFile, uint8_t value))m_lib.loadFunction("MP4SetAudioProfileLevel");
	yang_MP4AddAudioTrack =(MP4TrackId (*)(MP4FileHandle hFile, uint32_t timeScale,MP4Duration sampleDuration, uint8_t audioType))m_lib.loadFunction("MP4AddAudioTrack");
	yang_MP4AddH264VideoTrack = (MP4TrackId (*)(MP4FileHandle hFile,uint32_t timeScale, MP4Duration sampleDuration, uint16_t width,
			uint16_t height, uint8_t AVCProfileIndication,uint8_t profile_compat,uint8_t AVCLevelIndication,
			uint8_t sampleLenFieldSizeMinusOne))m_lib.loadFunction("MP4AddH264VideoTrack");
	yang_MP4SetTrackESConfiguration =(bool (*)(MP4FileHandle hFile, MP4TrackId trackId,const uint8_t *pConfig, uint32_t configSize))m_lib.loadFunction("MP4SetTrackESConfiguration");
	yang_MP4AddH264SequenceParameterSet =(void (*)(MP4FileHandle hFile, MP4TrackId trackId,const uint8_t *pSequence, uint16_t sequenceLen)) m_lib.loadFunction(
					"MP4AddH264SequenceParameterSet");
	yang_MP4AddH264PictureParameterSet =(void (*)(MP4FileHandle hFile, MP4TrackId trackId,
					const uint8_t *pPict, uint16_t pictLen)) m_lib.loadFunction("MP4AddH264PictureParameterSet");
	yang_MP4WriteSample = (bool (*)(MP4FileHandle hFile, MP4TrackId trackId,const uint8_t *pBytes, uint32_t numBytes, MP4Duration duration,
			MP4Duration renderingOffset, bool isSyncSample)) m_lib.loadFunction("MP4WriteSample");
	yang_MP4Close =(void (*)(MP4FileHandle hFile, uint32_t flags)) m_lib.loadFunction("MP4Close");

	yang_MP4AddH265VideoTrack=(MP4TrackId (*)(
	    MP4FileHandle hFile,
	    uint32_t      timeScale,
	    MP4Duration   sampleDuration,
	    uint16_t      width,
	    uint16_t      height,
	    uint8_t       isIso)) m_lib.loadFunction("MP4AddH265VideoTrack");
	yang_MP4AddH265SequenceParameterSet=(void (*)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pSequence,
	    uint16_t       sequenceLen )) m_lib.loadFunction("MP4AddH265SequenceParameterSet");
	yang_MP4AddH265PictureParameterSet=(void (*)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pPict,
	    uint16_t       pictLen )) m_lib.loadFunction("MP4AddH265PictureParameterSet");
	yang_MP4AddH265VideoParameterSet=(void (*) (MP4FileHandle hFile,
										 MP4TrackId trackId,
										 const uint8_t *pSequence,
										 uint16_t sequenceLen)) m_lib.loadFunction("MP4AddH265VideoParameterSet");
	yang_MP4GetTrackTimeScale=(uint32_t (*) (
		    MP4FileHandle hFile,
		    MP4TrackId    trackId )) m_lib.loadFunction("MP4GetTrackTimeScale");
	yang_MP4SetTrackTimeScale=(bool (*) (
		    MP4FileHandle hFile,
		    MP4TrackId    trackId,
		    uint32_t      value )) m_lib.loadFunction("MP4SetTrackTimeScale");
}

void YangMp4File::unloadLib() {
	yang_MP4AddH265VideoTrack=NULL;
	yang_MP4AddH265SequenceParameterSet=NULL;
	yang_MP4AddH265PictureParameterSet=NULL;
	yang_MP4AddH265VideoParameterSet=NULL;
	yang_MP4TagsAlloc = NULL;
    yang_MP4TagsFree=NULL;
	yang_MP4Create = NULL;
	yang_MP4TagsFetch = NULL;
	yang_MP4TagsSetSortArtist = NULL;
	yang_MP4TagsStore = NULL;
	yang_MP4SetTimeScale = NULL;
	yang_MP4SetVideoProfileLevel = NULL;
	yang_MP4SetAudioProfileLevel = NULL;
	yang_MP4AddAudioTrack = NULL;
	yang_MP4AddH264VideoTrack = NULL;
	yang_MP4SetTrackESConfiguration = NULL;
	yang_MP4AddH264SequenceParameterSet = NULL;
	yang_MP4AddH264PictureParameterSet = NULL;
	yang_MP4WriteSample = NULL;
	yang_MP4Close = NULL;
}
YangMp4File::~YangMp4File(void) {
	closeMp4();
	unloadLib();
	m_lib.unloadObject();
}

YangMp4File::YangMp4File(char *fileName, YangVideoInfo *pcontext) {
	m_context = pcontext;
	m_MP4hFile = MP4_INVALID_FILE_HANDLE;
	m_mp4Audiotrack = 0;
	m_mp4Videotrack = 0;
	m_ntracks = 0, m_trackno = 0;
	m_ndiscs = 0, m_discno = 0;
	m_newtick = 0;
	m_oldtick = 0;
	m_newalltick = 0, m_oldalltick = 0;
	m_tmptick = 0;
	m_tick = 0;
	m_cou = 0;
	m_interval = 2 * (1000 / m_context->frame) * 90;
	m_interval1 = 2 * (1000 / m_context->frame);

	m_artist = NULL;

	m_total_samples = 0;
	m_encoded_samples = 0;
	m_delay_samples = 0;
	m_frameSize = 0;
	//hEncoder=NULL;

	yang_trace("\ncreate mp4 file======%s", fileName);
	m_lib.loadObject("libmp4v2");
	loadLib();
	//MP4Create
	m_MP4hFile = yang_MP4Create(fileName, 0);

}
void YangMp4File::init(uint8_t *p_spsBuf, int32_t p_spsLen) {

	const MP4Tags *tags = yang_MP4TagsAlloc();
	yang_MP4TagsFetch(tags, m_MP4hFile);
	yang_MP4TagsStore(tags, m_MP4hFile);
	yang_MP4SetTimeScale(m_MP4hFile, 90000);
	m_mp4Audiotrack = yang_MP4AddAudioTrack(m_MP4hFile, 44100, 1024,MP4_MPEG4_AUDIO_TYPE);
	// MP4SetAudioProfileLevel(MP4hFile, 0x0F);
	//mp4Videotrack = MP4AddH264VideoTrack(MP4hFile, 90000, 90000/config.Frame_Num, config.Video_Width,config.Video_Height,0x42,0xc0,0x1f,3);
	//MP4AddH265VideoTrack
	if(m_context->videoEncoderType==1){
		//printf("\n**********MP4AddH265VideoTrack***********\n");
		m_mp4Videotrack = yang_MP4AddH265VideoTrack(m_MP4hFile, 90000, -1,
					m_context->width, m_context->height, 0);

	}else{
	m_mp4Videotrack = yang_MP4AddH264VideoTrack(m_MP4hFile, 90000, -1,
			m_context->width, m_context->height, //0x42, 0xc0, 0x1f, 3);
			*(p_spsBuf+1), *(p_spsBuf+2), *(p_spsBuf+3), 3);
	}
	m_frameSize = 1024;
	m_delay_samples = 1024;
    if(tags) yang_MP4TagsFree(tags);
    tags=NULL;


}
void YangMp4File::WriteVideoInfo(uint8_t *p_vpsBuf,int32_t p_vpsLen,uint8_t *p_spsBuf, int32_t p_spsLen,uint8_t *p_ppsBuf, int32_t p_ppsLen) {
	//for(int32_t i=0;i<p_spsLen;i++) printf("%02x,",*(p_spsBuf+i));
	//printf("\n");
	//for(int32_t i=0;i<p_ppsLen;i++) printf("%02x,",*(p_ppsBuf+i));
	//MP4AddH265PictureParameterSet()

	if(m_context->videoEncoderType==1){
		//MP4AddH265VideoParameterSet
		//MP4SetVideoProfileLevel
	//	yang_MP4SetVideoProfileLevel(m_MP4hFile, *(p_spsBuf));//0x7f);
		//printf("\n****************MP4AddH265VideoParameterSet**************************\n");0x08

		yang_MP4SetVideoProfileLevel(m_MP4hFile, 0x08);//0x7f);
		yang_MP4AddH265VideoParameterSet(m_MP4hFile, m_mp4Videotrack, p_vpsBuf,p_vpsLen);
		yang_MP4AddH265SequenceParameterSet(m_MP4hFile, m_mp4Videotrack, p_spsBuf,p_spsLen);
		yang_MP4AddH265PictureParameterSet(m_MP4hFile, m_mp4Videotrack, p_ppsBuf,	p_ppsLen);
		yang_MP4SetTrackTimeScale(m_MP4hFile, m_mp4Videotrack,90000);
		//printf("\nvpsLen=%d,spsLen=%d,ppsLen=%d",p_vpsLen,p_spsLen,p_ppsLen);

	}else{
		yang_MP4SetVideoProfileLevel(m_MP4hFile, *(p_spsBuf+1));//0x7f);
		yang_MP4AddH264SequenceParameterSet(m_MP4hFile, m_mp4Videotrack, p_spsBuf,p_spsLen);
		yang_MP4AddH264PictureParameterSet(m_MP4hFile, m_mp4Videotrack, p_ppsBuf,	p_ppsLen);
	}
	//printf("\n1****************timescale====%d**************************\n",yang_MP4GetTrackTimeScale(m_MP4hFile, m_mp4Videotrack));

}
void YangMp4File::WriteAudioInfo(uint8_t *pasc, unsigned long pasclen,uint8_t *buf1, int32_t buflen) {
	yang_MP4SetAudioProfileLevel(m_MP4hFile, 0x0f);
	yang_MP4SetTrackESConfiguration(m_MP4hFile, m_mp4Audiotrack, pasc, pasclen);
}
int32_t YangMp4File::WriteAudioPacket(YangFrame* audioFrame) {
	yang_MP4WriteSample(m_MP4hFile, m_mp4Audiotrack, audioFrame->payload, audioFrame->nb, -1, 0, 0);
	m_encoded_samples += m_frameSize;
	return 1;
}

int32_t YangMp4File::WriteVideoPacket(YangFrame* vidoeFrame) {

	yang_MP4WriteSample(m_MP4hFile, m_mp4Videotrack, vidoeFrame->payload, vidoeFrame->nb, vidoeFrame->timestamp, 0,	0);

	return 1;
}

void YangMp4File::closeMp4() {
	if (m_MP4hFile != NULL)
		yang_MP4Close(m_MP4hFile, 0);
	m_MP4hFile = NULL;
}

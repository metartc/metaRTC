//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangMp4File____
#define __YangMp4File____
#include <yangutil/yangavinfotype.h>

#include "faac.h"
#include "yangutil/sys/YangLoadLib.h"
#include "mp4v2/mp4v2.h"


class YangMp4File{

public :
	YangMp4File( char* fileName,YangVideoInfo *pvideo);
	~YangMp4File();
	MP4FileHandle m_MP4hFile ;
    MP4TrackId  m_mp4Audiotrack ;
    MP4TrackId  m_mp4Videotrack ;
    uint32_t  m_ntracks, m_trackno ;
    uint32_t  m_ndiscs , m_discno ;
    const char *m_artist ;

	double m_newtick,m_oldtick,m_newalltick,m_oldalltick,m_tmptick;
	int32_t m_tick,m_interval,m_interval1;
    uint32_t  m_total_samples;
    uint32_t  m_encoded_samples;
    uint32_t  m_delay_samples;
    uint32_t  m_frameSize;
	int32_t m_cou;



	void init(uint8_t *p_spsBuf, int32_t p_spsLen);
	void closeMp4();

	int32_t WriteVideoPacket(YangFrame* videoFrame);
	int32_t WriteAudioPacket(YangFrame* audioFrame);
	void WriteVideoInfo(uint8_t *p_vpsBuf,int32_t p_vpsLen,uint8_t *p_spsBuf, int32_t p_spsLen,uint8_t *p_ppsBuf, int32_t p_ppsLen);
	void WriteAudioInfo(uint8_t *pasc,unsigned long pasclen,uint8_t* buf1,int32_t buflen);
	double m_framerate,m_bitrate, m_level_idc;
private:
	YangVideoInfo *m_context;
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	const MP4Tags* (*yang_MP4TagsAlloc)( void );
    void (*yang_MP4TagsFree)( const MP4Tags* tags );
	MP4FileHandle (*yang_MP4Create)(const char* fileName,uint32_t flags);
	bool (*yang_MP4TagsFetch)( const MP4Tags* tags, MP4FileHandle hFile );
	bool (*yang_MP4TagsSetSortArtist)      ( const MP4Tags*, const char* );
	bool (*yang_MP4TagsStore)( const MP4Tags* tags, MP4FileHandle hFile );
	bool (*yang_MP4SetTimeScale)( MP4FileHandle hFile, uint32_t value );
	void (*yang_MP4SetVideoProfileLevel)( MP4FileHandle hFile, uint8_t value );
	void (*yang_MP4SetAudioProfileLevel)( MP4FileHandle hFile, uint8_t value );
	MP4TrackId (*yang_MP4AddAudioTrack)( MP4FileHandle hFile, uint32_t      timeScale,
	    MP4Duration   sampleDuration,  uint8_t       audioType );
	MP4TrackId (*yang_MP4AddH264VideoTrack)(
	    MP4FileHandle hFile, uint32_t timeScale,MP4Duration sampleDuration,uint16_t width,uint16_t  height,	uint8_t   AVCProfileIndication
		,uint8_t profile_compat,  uint8_t  AVCLevelIndication,  uint8_t  sampleLenFieldSizeMinusOne );
	bool (*yang_MP4SetTrackESConfiguration)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pConfig,
	    uint32_t       configSize );
	void (*yang_MP4AddH264SequenceParameterSet)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pSequence,
	    uint16_t       sequenceLen );
	void (*yang_MP4AddH264PictureParameterSet)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pPict,
	    uint16_t       pictLen );
	bool (*yang_MP4WriteSample)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pBytes,
	    uint32_t       numBytes,
	    MP4Duration    duration,
	    MP4Duration    renderingOffset,
	    bool           isSyncSample);
	void (*yang_MP4Close)(
	    MP4FileHandle hFile,
	    uint32_t    flags);

	MP4TrackId (*yang_MP4AddH265VideoTrack)(
	    MP4FileHandle hFile,
	    uint32_t      timeScale,
	    MP4Duration   sampleDuration,
	    uint16_t      width,
	    uint16_t      height,
	    uint8_t       isIso);
	void (*yang_MP4AddH265SequenceParameterSet)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pSequence,
	    uint16_t       sequenceLen );
	void (*yang_MP4AddH265PictureParameterSet)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pPict,
	    uint16_t       pictLen );
	void (*yang_MP4AddH265VideoParameterSet) (MP4FileHandle hFile,
										 MP4TrackId trackId,
										 const uint8_t *pSequence,
										 uint16_t sequenceLen);
	uint32_t (*yang_MP4GetTrackTimeScale)(
	    MP4FileHandle hFile,
	    MP4TrackId    trackId );
	bool (*yang_MP4SetTrackTimeScale)(
	    MP4FileHandle hFile,
	    MP4TrackId    trackId,
	    uint32_t      value );
};
#endif

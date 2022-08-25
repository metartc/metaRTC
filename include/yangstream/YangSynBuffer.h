//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPLAYER_INCLUDE_YANGAVSYN_H_
#define YANGPLAYER_INCLUDE_YANGAVSYN_H_
#include <yangutil/sys/YangTime.h>

#include <yangutil/yangavtype.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangutil/buffer/YangVideoBuffer.h>


enum YangSynType{
	YANG_SYNC_AUDIO_MASTER, /* default choice */
	YANG_SYNC_VIDEO_MASTER,
	YANG_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};
#define Yang_Max_Audio_Intervaltime 25
#define Yang_Max_Video_Intervaltime 35
#define Yang_Video_Base_Update_Interval 600000//10*60*1000  //10m
#define Yang_Audio_Base_Update_Interval 5000//10*60*1000  //10m
#define Yang_Video_Cache_time 35
#define Yang_Audio_Cache_time 5
class YangSynBuffer
{
public:
    YangSynBuffer();
    virtual ~YangSynBuffer();
    int32_t m_uid;
    int32_t m_width;
    int32_t m_height;
    void setTranstype(int transtype);
    void setAudioClock(int paudioclock);
    void setVideoClock(int pvideoclock);
    void setVideoCacheTime(int pctime);

    void resetVideoClock();
    void resetAudioClock();

public:
    uint8_t* getVideoRef(YangFrame* pframe);
    uint8_t* getAudioRef(YangFrame* audioFrame);
    int32_t getAudioSize();
    int32_t getVideoSize();


    void setInAudioBuffer(YangAudioPlayBuffer *paudioList);
    void setInVideoBuffer(YangVideoBuffer *pbuf);


    void initClock();
    void setClock();

    int playAudioFrame(int64_t pts);
    int playVideoFrame(YangFrame* frame);
    double compute_target_delay(double delay);
    void video_refresh(int32_t pisaudio, double *remaining_time);
    int32_t m_maxAudioMinus;
    int32_t m_maxVideoMinus;
private:
    bool m_isFirstVideo;
    bool m_isFirstAudio;
    int32_t m_video_time_state;
    void updateBaseTimestamp(int64_t pts);
    void updateVideoBaseTimestamp(int64_t pts);
    void updateAudioBaseTimestamp(int64_t pts);
private:

    YangSynType m_synType;
    int32_t m_paused;
    int64_t m_baseClock;

    int64_t m_audio_startClock;
    int32_t m_audioClock;
    int64_t m_audioBase;
    int64_t m_audioTime;
    int32_t m_audioMinus;
    int32_t m_audioDelay;
    int32_t m_lostAudioCount;
    int32_t m_audioNegativeCount;
    int32_t m_audioTimeoutCount;


    int64_t m_videoBase;
    int32_t m_videoClock;
    int64_t m_videoTime;
    int32_t m_videoMinus;
    int32_t m_videoNegativeCount;
    int32_t m_videoTimeoutCount;
    int32_t m_videoCacheTime;
    int64_t m_video_startClock;

    int64_t m_pre_audioTime;
    int64_t m_pre_videoTime;

    int m_transtype;


private:
    YangVideoBuffer *m_videoBuffer;
    YangAudioPlayBuffer* m_audioBuffer;
};

#endif /* YANGPLAYER_INCLUDE_YANGAVSYN_H_ */

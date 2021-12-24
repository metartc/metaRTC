#ifndef SRC_YANGRTP_YANGMIXQUEUE_H_
#define SRC_YANGRTP_YANGMIXQUEUE_H_
#include <yangutil/yangavtype.h>
#include <map>
#define YANG_MIX_CORRECT_PURE_AV_VIDEO 10
#define YANG_MIX_CORRECT_PURE_AV_AUDIO 5
class YangMixQueue
{
private:
    uint32_t nb_videos;
    uint32_t nb_audios;
    int m_audio_size;
    int m_video_size;
    std::multimap<int64_t, YangMessage*> msgs;
public:
    YangMixQueue();
    virtual ~YangMixQueue();
public:
    virtual void clear();
    virtual void push(YangMessage* msg);
    virtual YangMessage* pop();
    void setAudioSize(uint32_t asize);
    void setVideoSize(uint32_t vsize);
};
#endif /* SRC_YANGRTP_YANGMIXQUEUE_H_ */

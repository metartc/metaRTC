#include <yangrtp/YangMixQueue.h>
#include <yangutil/yangtype.h>

YangMixQueue::YangMixQueue()
{
    nb_videos = 0;
    nb_audios = 0;
    m_audio_size=YANG_MIX_CORRECT_PURE_AV_AUDIO;
    m_video_size=YANG_MIX_CORRECT_PURE_AV_VIDEO;
}

YangMixQueue::~YangMixQueue()
{
    clear();
}
void YangMixQueue::setAudioSize(uint32_t asize){
	m_audio_size=asize;
}
    void YangMixQueue::setVideoSize(uint32_t vsize){
    	m_video_size=vsize;
    }
void YangMixQueue::clear()
{
    std::multimap<int64_t, YangMessage*>::iterator it;
    for (it = msgs.begin(); it != msgs.end(); ++it) {
    	YangMessage* msg = it->second;
        yang_delete(msg);
    }
    msgs.clear();

    nb_videos = 0;
    nb_audios = 0;
}

void YangMixQueue::push(YangMessage* msg)
{
    msgs.insert(std::make_pair(msg->timestamp, msg));

    if (msg->mediaType==1) {
        nb_videos++;
    } else {
        nb_audios++;
    }
}

YangMessage* YangMixQueue::pop()
{
    bool mix_ok = false;

    // pure video
    if (nb_videos >= m_video_size && nb_audios == 0) {
        mix_ok = true;
    }

    // pure audio
    if (nb_audios >= m_audio_size && nb_videos == 0) {
        mix_ok = true;
    }

    // got 1 video and 1 audio, mix ok.
    if (nb_videos >= 1 && nb_audios >= 1) {
        mix_ok = true;
    }

    if (!mix_ok) {
        return NULL;
    }

    // pop the first msg.
    std::multimap<int64_t, YangMessage*>::iterator it = msgs.begin();
    YangMessage* msg = it->second;
    msgs.erase(it);

    if (msg->mediaType==1) {
        nb_videos--;
    } else {
        nb_audios--;
    }

    return msg;
}

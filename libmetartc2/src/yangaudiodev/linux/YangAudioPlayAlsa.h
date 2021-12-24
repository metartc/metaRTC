/*
 * YangAudioPlayAlsa.h
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#ifndef YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_
#define YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_

#ifndef _WIN32
#include <alsa/asoundlib.h>


#include <yangaudiodev/YangAudioPlay.h>

class YangAudioPlayAlsa:public YangAudioPlay{
public:
	YangAudioPlayAlsa(YangContext *pcontext);
	~YangAudioPlayAlsa();

    int init();



protected:
	void startLoop();
	void stopLoop();

private:
	 int32_t m_loops;

private:

    int32_t m_contextt;
	snd_pcm_t *m_handle;
	snd_pcm_uframes_t m_frames;


	void closeAudio();
	int32_t ret;

};
#endif
#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */

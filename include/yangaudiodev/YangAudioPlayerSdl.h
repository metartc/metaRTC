
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPLAYER_INCLUDE_YANGAUDIOPLAYSDL1_H_
#define YANGPLAYER_INCLUDE_YANGAUDIOPLAYSDL1_H_

#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangThread2.h>

#include <SDL2/SDL.h>

class YangAudioPlayerSdl:public YangThread{
public:
	YangAudioPlayerSdl(YangAudioInfo *pcontext);
	~YangAudioPlayerSdl();
	void init();
        void*  (*yang_SDL_yang_memset)(SDL_OUT_BYTECAP(len) void *dst, int32_t c, size_t len);
        void  (*yang_SDL_MixAudio)(Uint8 * dst, const Uint8 * src, Uint32 len, int32_t volume);
	void setAudioList(YangAudioPlayBuffer *pal);
	int32_t m_isStart;
	void stop();

	int32_t m_frames;
	int32_t m_channel;
	int32_t m_sample;
	static void fill_audio(void *udata, Uint8 *stream, int32_t len);
protected:
	void startLoop();
	void stopLoop();
	void run();

private:
	YangAudioInfo *m_context;
	YangAudioPlayBuffer *m_in_audioBuffer;
	static YangAudioPlayerSdl* m_instance;
	int32_t isInit;

	void closeAudio();
	int32_t ret;
	int32_t m_size;
	int32_t m_loops;

	SDL_AudioSpec wanted;
	void playSDL(uint8_t *p_data);
	void initSDL();
	void startLoopSDL();
	YangLoadLib m_lib,m_lib1;
	void loadLib();
	void unloadLib();
        int32_t  (*yang_SDL_Init)(Uint32 flags);
        void  (*yang_SDL_Delay)(Uint32 ms);

        const char * (*yang_SDL_GetError)(void);

        int32_t  (*yang_SDL_OpenAudio)(SDL_AudioSpec * desired, SDL_AudioSpec * obtained);

        void  (*yang_SDL_PauseAudio)(int32_t pause_on);
        void  (*yang_SDL_CloseAudio)(void);

};

#endif /* YANGPLAYER_INCLUDE_YANGAUDIOPLAYSDL_H_ */

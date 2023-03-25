//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGCAPTURE_YANGRENDERAUDIODATA_H_
#define INCLUDE_YANGCAPTURE_YANGRENDERAUDIODATA_H_
#include <yangutil/yangavinfotype.h>

#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangavutil/audio/YangAudioUtil.h>

#include <yangstream/YangSynBuffer.h>


class YangAudioRenderData {
public:
    YangAudioRenderData();
    virtual ~YangAudioRenderData();

public:

    YangSynBuffer* m_syn;
    YangPreProcess* m_preProcess;
    YangAudioResample m_res;
    void setAec();
    void setRenderLen(int plen);
    void setContext(YangSynBufferManager* streams);
    void initRender(int psample,int pchannel);
    void initPlay(int psample,int pchannel);
    uint8_t* getAudioRef(YangFrame* pframe);
    uint8_t* getRenderAudioData(int len);
    uint8_t* getAecAudioData();
    void setInAudioBuffer(YangSynBuffer *pal);


private:
    bool m_hasAec;
    int m_aecBufLen;
    int m_mixPos;
    int m_renderLen;
    uint8_t* m_aecBuf;


    uint8_t* m_cache;
    int m_cacheLen;
    int m_size;
    int m_pos;

    uint8_t* m_mixBuf;
    YangSynBufferManager* m_syns;

    YangFrame m_audioFrame;



private:

    uint8_t* getAudioData(YangFrame* frame);

    void setAudioData(YangFrame* frame);


};

#endif /* INCLUDE_YANGCAPTURE_YANGRENDERAUDIODATA_H_ */

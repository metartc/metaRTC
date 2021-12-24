/*
 * YangAudioDataManager.h
 *
 *  Created on: 2021年9月21日
 *      Author: yang
 */

#ifndef INCLUDE_YANGCAPTURE_YANGRENDERAUDIODATA_H_
#define INCLUDE_YANGCAPTURE_YANGRENDERAUDIODATA_H_
#include <stdint.h>
#include <vector>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangavutil/audio/YangAudioUtil.h>

#include <yangstream/YangSynBuffer.h>
using namespace std;

class YangAudioRenderData {
public:
    YangAudioRenderData();
    virtual ~YangAudioRenderData();

public:
    vector<YangSynBuffer*> *m_in_audioBuffers;
    YangSynBuffer* m_syn;
    YangPreProcess* m_preProcess;
    YangAudioResample m_res;
    void setAec();
    void setRenderLen(int plen);

    void initRender(int psample,int pchannel);
    void initPlay(int psample,int pchannel);
    uint8_t* getAudioRef(YangFrame* pframe);
    uint8_t* getRenderAudioData(int len);
    uint8_t* getAecAudioData();
    void setInAudioBuffer(YangSynBuffer *pal);
    void setInAudioBuffers(vector<YangSynBuffer*> *pal);

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

    YangFrame m_audioFrame;

    YangAudioMix m_mix;

private:

    uint8_t* getAudioData(YangFrame* frame);

    void setAudioData(YangFrame* frame);
    bool hasData();

};

#endif /* INCLUDE_YANGCAPTURE_YANGRENDERAUDIODATA_H_ */
